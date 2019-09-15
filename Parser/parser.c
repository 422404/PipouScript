/**
 * @file parser.c
 * Parser implementation
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parser/include/parser.h"
#include "Common/include/error.h"
#include "lexer.h"
#include "vector.h"
#include "misc.h"
#include "ast.h"
#include "location.h"

/**
 * Give some lookahead on the lexer token stream
 * @returns The next token in the stream with lookahead
 *          The token must be freed by calling Parser_ConsumeLookahead()
 */
static token_t * Parser_NextToken(parser_t * parser, bool preserve_whitespaces,
                                  bool preserve_comments) {
    token_t * token;
    bool must_loop = false;

    if (!parser) Err_Throw(Err_New("NULL pointer to parser"));

    do {
        if (parser->token_lookahead_index < Vec_GetLength(parser->token_lookahead)) {
            token = Vec_GetAt(parser->token_lookahead, parser->token_lookahead_index);
            parser->token_lookahead_index++;
        } else {
            token = Lex_NextToken(parser->lexer, true, true);
            if (token) {
                Vec_Append(parser->token_lookahead, token);
                parser->token_lookahead_index++;
            } else if (Lex_GetStatus(parser->lexer) == LEX_ERROR) {
                parser->error = Lex_GetError(parser->lexer);
                parser->status = PARSER_ERROR;
            }
        }
        must_loop = token
                && ((!preserve_whitespaces && Token_IsWhitespace(token))
                    || (!preserve_comments && token->type == TOKTYPE_COMMENT));
    } while (must_loop);

    return token;
}

static void Parser_PushBackTokenList(parser_t * parser) {
    if (parser->token_lookahead_index >= 1) parser->token_lookahead_index--;
}

/**
 * Consume the lookahead
 */
static void Parser_ConsumeLookahead(parser_t * parser) {
    token_t * token;

    while (Vec_GetLength(parser->token_lookahead)) {
        token = Vec_Pop(parser->token_lookahead);
        if (token) Token_Free(token);
    }
    parser->token_lookahead_index = 0;
}

static loc_t Parser_CurrentLocation(parser_t * parser) {
    loc_t loc;

    if (parser->token_lookahead_index < Vec_GetLength(parser->token_lookahead)) {
        loc = ((token_t *)Vec_GetAt(parser->token_lookahead, parser->token_lookahead_index))->span.start;
    } else {
        loc = parser->lexer->pos;
    }

    return loc;
}

/**
 * Allocates a new parser
 * @param[in] buffer      The buffer where the code to be parsed is located
 * @param     length      Length of the buffer
 * @param[in] filename    Name of the file that contains the code
 * @param     module_mode Indicates if the code resides in a module
 *                        or in the main source file
 * @returns               The newly allocated parser
 */
parser_t * Parser_New(char * buffer, size_t length, char * filename, bool module_mode) {
    parser_t * parser;

    parser = (parser_t *)malloc(sizeof(parser_t));
    if (parser) {
        parser->filename = filename;
        parser->module_mode = module_mode;
        parser->lexer = Lex_New(buffer, length, filename);
        parser->token_lookahead = Vec_New();
        parser->token_lookahead_index = 0;
        parser->error = NULL;
        parser->status = PARSER_OK;
    } else {
        Err_Throw(Err_New("Cannot allocate parser"));
    }

    return parser;
}

/**
 * Frees a previously allocated parser
 * @param[in] parser The parser to be freed
 */
void Parser_Free(parser_t * parser) {
    if (parser) {
        if (parser->lexer) Lex_Free(parser->lexer);
        if (parser->error) Err_Free(parser->error);
        Parser_ConsumeLookahead(parser);
        Vec_Free(parser->token_lookahead);
        free(parser);
    }
    else Err_Throw(Err_New("NULL pointer to parser"));
}

/**
 * Returns the status of the parser
 * @returns the status of the parser
 */
parser_status_t Parser_GetStatus(parser_t * parser) {
    parser_status_t status = PARSER_OK;
    if (parser) status = parser->status;
    else Err_Throw(Err_New("NULL pointer to parser"));
    return status;
}

/**
 * Returns the error that the parser encountered if any
 * @retval NULL if no error
 * @retval A pointer to an error if any
 */
error_t * Parser_GetError(parser_t * parser) {
    error_t * error = NULL;
    if (parser) error = parser->error;
    else Err_Throw(Err_New("NULL pointer to parser"));
    return error;
}

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser       The parser used to generate the AST
 * @param     module_scope Whether we parse a module
 * @returns          The AST root node
 */
ast_node_t * Parser_CreateAST(parser_t * parser, bool module_scope) {
    ast_node_t * node, * value;

    node = ASTNode_New(NODE__ROOT_);

    while ((value = Parser_ParseStatement(parser, module_scope))
            && Parser_GetStatus(parser) == PARSER_OK) {
        Vec_Append(node->as_root.statements, value);
    }
    
    if (Parser_GetStatus(parser) == PARSER_ERROR) {
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseIdentifier(parser_t * parser, bool directly) {
    token_t * token;
    ast_node_t * node = NULL;
    
    token = Parser_NextToken(parser, directly, directly);
    if (token) {
        if (token->type == TOKTYPE_IDENT) {
            node = ASTNode_New(NODE_IDENTIFIER);
            node->as_ident.value = strdup(token->value);
        } else {
            Parser_PushBackTokenList(parser);
        }
    }

    return node;
}

ast_node_t * Parser_ParseString(parser_t * parser) {
    token_t * token;
    ast_node_t * node = NULL;
    
    token = Parser_NextToken(parser, false, false);
    if (token) {
        if (token->type == TOKTYPE_STRING) {
            node = ASTNode_New(NODE_STRING);
            node->as_string.value = strdup(token->value + 1); // skip the first '"'
            node->as_string.value[strlen(node->as_ident.value) - 1] = '\0'; // remove the last '"'
        } else {
            Parser_PushBackTokenList(parser);
        }
    }

    return node;
}

ast_node_t * Parser_ParseInt(parser_t * parser) {
    token_t * token;
    ast_node_t * node = NULL;
    
    token = Parser_NextToken(parser, false, false);
    if (token) {
        if (token->type == TOKTYPE_INT) {
            node = ASTNode_New(NODE_INT);
            sscanf(token->value, "%d", &node->as_int.value);
        } else {
            Parser_PushBackTokenList(parser);
        }
    }

    return node;
}

ast_node_t * Parser_ParseDouble(parser_t * parser) {
    token_t * token;
    ast_node_t * node = NULL;
    
    token = Parser_NextToken(parser, false, false);
    if (token) {
        if (token->type == TOKTYPE_DOUBLE) {
            node = ASTNode_New(NODE_DOUBLE);
            sscanf(token->value, "%lf", &node->as_double.value);
        } else {
            Parser_PushBackTokenList(parser);
        }
    }

    return node;
}

ast_node_t * Parser_ParseObjFieldName(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_HASH,
        GOT_IDENT2,
        GOT_COLON
    };
    enum {
        NONE,
        NO_TOKEN,
        NO_IDENT
    };
    ast_node_t * node, * ident;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_OBJ_FIELD_NAME);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok) {
                    error_state = NO_TOKEN;
                } else if (tok->type == TOKTYPE_HASH) {
                    node->as_obj_field_name.is_msg_name = true;
                    state = GOT_HASH;
                } else {
                    Parser_PushBackTokenList(parser);
                    ident = Parser_ParseIdentifier(parser, false);
                    if (!ident) {
                        error_state = NO_IDENT;
                    } else {
                        Vec_Append(node->as_obj_field_name.components, ident);
                        state = GOT_IDENT;
                    }
                }
                break;
            
            case GOT_IDENT:
                must_loop = false;
                break;
            
            case GOT_HASH:
            case GOT_COLON:
                ident = Parser_ParseIdentifier(parser, true);
                if (!ident) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_obj_field_name.components, ident);
                    state = GOT_IDENT2;
                }
                break;
            
            case GOT_IDENT2:
                tok = Parser_NextToken(parser, true, true);
                if (tok && tok->type == TOKTYPE_COLON) {
                    state = GOT_COLON;
                } else {
                    must_loop = false;
                    if (tok) Parser_PushBackTokenList(parser);
                }
                break;

        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        bool is_hash = false;
        switch (state) {
            case START: // parser->error is already set if any error occured
            case GOT_IDENT2:
                break;
            
            case GOT_HASH:
                is_hash = true; // fallthrough
            case GOT_COLON:
                parser->error = Err_NewWithLocation(is_hash
                        ? "Expected a parameter name after '#'"
                        : "Expected another parameter name after ':'", loc);
                break;
            
            case GOT_IDENT: // no errors possible
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseExpr(parser_t * parser) {
    return  Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
}

/**
 * @param[in] parser
 * @param     type   NODE_OR_EXPR <= type <= NODE_FACTOR_EXPR
 * @retval an ast_node_t<ast_expr_t> * if there is atleast 2 components (a <op of type> b)
 * @retval an ast_node_t<T> * if only one component of type T
 */
ast_node_t * Parser_ParseBinaryExpr(parser_t * parser, ast_node_type_t type) {
    ast_node_t * node, * value;
    token_t * tok;
    ast_node_type_t inf_type = type + 1;
    bool must_loop = true;
    bool reset = false;
    size_t looahead_index;

    assert(NODE_OR_EXPR <= type && type <= NODE_FACTOR_EXPR);
    node = ASTNode_New(type);
    node->as_expr.op = 0;

    for (size_t i = 0; must_loop && Parser_GetStatus(parser) == PARSER_OK; i++) {
        looahead_index = parser->token_lookahead_index;
        value = type == NODE_FACTOR_EXPR
                ? Parser_ParseMsgPassExpr(parser)
                : (type == NODE_COMP_EXPR
                        || type == NODE_ARITH_EXPR
                        || type == NODE_EQ_EXPR) && reset
                    ? Parser_ParseBinaryExpr(parser, type)
                    : Parser_ParseBinaryExpr(parser, inf_type);
        if (!value) {
            if (i > 0 && !parser->error) {
                char buf[256];
                loc_t loc = Parser_CurrentLocation(parser);
                char * op_str;
                switch (node->as_expr.op) {
                    case TOKTYPE_PIPEPIPE:
                        op_str = "||";
                        break;
                    case TOKTYPE_AMPAMP:
                        op_str = "&&";
                        break;
                    case TOKTYPE_EQEQUAL:
                        op_str = "==";
                        break;
                    case TOKTYPE_NOTEQUAL:
                        op_str = "!=";
                        break;
                    case TOKTYPE_GEQUAL:
                        op_str = ">=";
                        break;
                    case TOKTYPE_LEQUAL:
                        op_str = "<=";
                        break;
                    case TOKTYPE_GREATER:
                        op_str = ">";
                        break;
                    case TOKTYPE_LOWER:
                        op_str = "<";
                        break;
                    case TOKTYPE_PLUS:
                        op_str = "+";
                        break;
                    case TOKTYPE_MINUS:
                        op_str = "-";
                        break;
                    case TOKTYPE_STAR:
                        op_str = "*";
                        break;
                    case TOKTYPE_SLASH:
                        op_str = "/";
                        break;
                    default:
                        op_str = "<op error>";
                        break;
                }
                snprintf(buf, 256, "Expected an expression after '%s'", op_str);
                parser->error = Err_NewWithLocation(buf, loc);
                parser->status = PARSER_ERROR;
            }
            must_loop = false;
        } else {
            Vec_Append(node->as_expr.values, value);
            tok = Parser_NextToken(parser, false, false);
            if (!tok) {
                must_loop = false;
            } else {
                switch (type) {
                    case NODE_OR_EXPR:
                        must_loop = tok->type == TOKTYPE_PIPEPIPE;
                        break;
                    case NODE_AND_EXPR:
                        must_loop = tok->type == TOKTYPE_AMPAMP;
                        break;
                    case NODE_EQ_EXPR:
                        must_loop = tok->type == TOKTYPE_EQEQUAL
                                || tok->type == TOKTYPE_NOTEQUAL;
                        break;
                    case NODE_COMP_EXPR:
                        must_loop = tok->type == TOKTYPE_GEQUAL
                                || tok->type == TOKTYPE_LEQUAL
                                || tok->type == TOKTYPE_GREATER
                                || tok->type == TOKTYPE_LOWER;
                        break;
                    case NODE_ARITH_EXPR:
                        must_loop = tok->type == TOKTYPE_PLUS
                                || tok->type == TOKTYPE_MINUS;
                        break;
                    case NODE_TERM_EXPR:
                        must_loop = tok->type == TOKTYPE_STAR;
                        break;
                    case NODE_FACTOR_EXPR:
                        must_loop = tok->type == TOKTYPE_SLASH;
                        break;
                    default:
                        break;
                }
                /*
                 * For NODE_EQ_EXPR, NODE_COMP_EXPR and NODE_ARITH_EXPR if 
                 * the second parsed token is not the same as the precedent
                 * (it's always the case for the other types) we go back
                 * before we parsed the last expression and we reparse it
                 * with the same type this function was given in parameter
                 * instead of the inf_type
                 * 
                 * Exemple:
                 * a + b - c
                 *       ^
                 * The previous expression was parsed as NODE_TERM_EXPR
                 * The previous parsed token was '+' so we go back in time
                 * 
                 * a + b - c
                 *     ^
                 * And instead of parsing a NODE_TERM_EXPR we parse a NODE_ARITH_EXPR
                 * So we have:
                 *      +
                 *     / \
                 *    a   -
                 *       / \
                 *      b   c
                 * 
                 * If we had a + b + c it would have been:
                 *      +
                 *     /|\
                 *    a b c
                 */
                if (must_loop) {
                    if (i == 0) {
                        node->as_expr.op = tok->type;
                    } else if (i > 0 && node->as_expr.op != tok->type) {
                        reset = true;
                        parser->token_lookahead_index = looahead_index;
                        ast_node_t * node2 = Vec_Pop(node->as_expr.values);
                        ASTNode_Free(node2);
                    }
                } else {
                    Parser_PushBackTokenList(parser);
                }
            }
        }
    }

    if (Parser_GetStatus(parser) == PARSER_ERROR
            || Vec_GetLength(node->as_expr.values) == 0) {
        ASTNode_Free(node);
        node = NULL;
    } else if (Vec_GetLength(node->as_expr.values) == 1) {
        /* 
         * we shorten the ast tree by skipping nodes that were
         * the only children of their parent
         */
        ast_node_t * node2 = Vec_Pop(node->as_expr.values);
        ASTNode_Free(node);
        node = node2;
    }

    return node;
}

/**
 * @retval an ast_node_t<ast_msg_pass_expr_t> * if there is atleast 2 components (a b: "hello" or a b)
 * @retval an ast_node_t<T> * if only one component of type T
 */
ast_node_t * Parser_ParseMsgPassExpr(parser_t * parser) {
    enum {
        START,
        GOT_ATOM_EXPR,
        GOT_IDENT,
        GOT_COLON,
        GOT_ATOM_EXPR2,
        GOT_IDENT2,
        GOT_COLON2,
        GOT_ATOM_EXPR3,
        GOT_IDENT3
    };
    enum {
        NONE,
        NO_ATOM_EXPR,
        NO_IDENT,
        NO_COLON,
        ATOM_EXPR_ERROR,
        NO_SPACE_BEFORE_MESSAGE
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_MSG_PASS_EXPR);

    do {
        switch (state) {
            case START:
                value = Parser_ParseAtomExpr(parser);
                if (value) {
                    state = GOT_ATOM_EXPR;
                    Vec_Append(node->as_msg_pass_expr.components, value);
                } else {
                    error_state = NO_ATOM_EXPR;
                }
                break;
            
            case GOT_ATOM_EXPR:
                {
                    bool got_spacing = false;
                    // we want spacing before message name
                    tok = Parser_NextToken(parser, true, true);
                    if (tok && (Token_IsWhitespace(tok) || tok->type == TOKTYPE_COMMENT)) {
                        got_spacing = true;
                    }
                    if (tok) Parser_PushBackTokenList(parser);
                    value = Parser_ParseIdentifier(parser, false);
                    if (value) {
                        if (got_spacing) {
                            Vec_Append(node->as_msg_pass_expr.components, value);
                            state = GOT_IDENT;
                        } else {
                            error_state = NO_SPACE_BEFORE_MESSAGE;
                            ASTNode_Free(value);
                            Parser_PushBackTokenList(parser);
                        }
                    } else {
                        must_loop = false;
                    }
                }
                break;
            
            case GOT_IDENT:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_COLON) {
                    must_loop = false;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_COLON;
                }
                break;
            
            case GOT_COLON:
                value = Parser_ParseAtomExpr(parser);
                if (value) {
                    state = GOT_ATOM_EXPR2;
                    Vec_Append(node->as_msg_pass_expr.components, value);
                } else {
                    error_state = NO_ATOM_EXPR;
                }
                break;
            
            case GOT_ATOM_EXPR2:
            case GOT_ATOM_EXPR3:
                value = Parser_ParseIdentifier(parser, false);
                if (value) {
                    Vec_Append(node->as_msg_pass_expr.components, value);
                    state = GOT_IDENT2;
                } else {
                    must_loop = false;
                }
                break;
            
            case GOT_IDENT2:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_COLON) {
                    error_state = NO_COLON;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_COLON2;
                }
                break;
            
            case GOT_COLON2:
                value = Parser_ParseAtomExpr(parser);
                if (value) {
                    state = GOT_ATOM_EXPR3;
                    Vec_Append(node->as_msg_pass_expr.components, value);
                } else {
                    error_state = NO_ATOM_EXPR;
                }
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
            case GOT_ATOM_EXPR2:
            case GOT_ATOM_EXPR3:
            case GOT_IDENT:
                break;

            case GOT_ATOM_EXPR:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected spacing before message name", loc);
                }
                break;

            case GOT_COLON:
            case GOT_COLON2:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected an expression after ':'", loc);
                }
                break;

            case GOT_IDENT2:
            case GOT_IDENT3:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a ':' after the parameter name", loc);
                }
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    } else if (Vec_GetLength(node->as_msg_pass_expr.components) == 1) {
        /* 
         * we shorten the ast tree by skipping nodes that were
         * the only children of their parent
         */
        ast_node_t * node2 = Vec_Pop(node->as_msg_pass_expr.components);
        ASTNode_Free(node);
        node = node2;
    }

    return node;
}

/**
 * Be sure to restore the lookahead index on return val = {NULL, NULL}
 */
ast_node_t * Parser_ParseDecl(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_COLEQUAL,
        GOT_EXPR,
        GOT_SEMICOLON
    };
    enum {
        NONE,
        NO_IDENT,
        NO_COLEQUAL,
        NO_EXPR,
        NO_SEMICOLON
    };
    ast_node_t * node, * ident, * expr;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_DECL);

    /** @todo eliminate code repetition */
    do {
        switch (state) {
            case START:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident) {
                    error_state = NO_IDENT;
                } else {
                    node->as_decl.lval = ident;
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_COLEQUAL) {
                    error_state = NO_COLEQUAL;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_COLEQUAL;
                }
                break;
            
            case GOT_COLEQUAL: // now we are sure it's a decl
                expr = Parser_ParseExpr(parser);
                if (!expr) {
                    error_state = NO_EXPR;
                } else {
                    node->as_decl.rval = expr;
                    state = GOT_EXPR;
                }
                break;
            
            case GOT_EXPR:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_SEMICOLON) {
                    error_state = NO_SEMICOLON;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_SEMICOLON;
                }
                break;
            
            case GOT_SEMICOLON:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
            case GOT_IDENT: // we are not a decl
            case GOT_SEMICOLON:
                break;
            
            case GOT_COLEQUAL: // no expr has been found
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected an expression after ':='", loc);
                }
                break;
            
            case GOT_EXPR: // no SEMICOLON has been found
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a ';' at the end of the declaration statement", loc);
                }
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseStatement(parser_t * parser, bool module_scope) {
    ast_node_t * node;
    ast_node_t * value;
    token_t * tok;
    loc_t loc;
    bool nothing_to_parse = false;
    size_t lookahead_index_backup;

    node = ASTNode_New(NODE_STATEMENT);

    tok = Parser_NextToken(parser, false, false);
    if (tok && tok->type == TOKTYPE_CIRCUMFLEX) {
        node->as_statement.is_return_expr = true;
        node->as_statement.is_local_return = false;
        node->as_statement.is_mod_statement = false;
        value = Parser_ParseExpr(parser);
        if (!value) {
            if (!parser->error) {
                parser->status = PARSER_ERROR;
                loc = Parser_CurrentLocation(parser);
                parser->error = Err_NewWithLocation("Expected an identifier after '^'", loc);
            }
        } else {
            node->as_statement.value = value;
        }
    } else if (Parser_GetStatus(parser) == PARSER_OK) {
        if (tok) Parser_PushBackTokenList(parser);
        /* - try to parse the complex ones first (decl and affect)
         * - save the lookahead index so we can rollback
         *   --> decl, affect and expr are ambiguous be cause they
         *       can start by an identifier
         */
        lookahead_index_backup = parser->token_lookahead_index;
        value = Parser_ParseDecl(parser);
        if (value) {
            node->as_statement.is_return_expr = false;
            node->as_statement.is_local_return = false;
            node->as_statement.is_mod_statement = true;
            node->as_statement.value = value;
        } else if (Parser_GetStatus(parser) == PARSER_OK) {
            // no decl so rollback lookahead index
            parser->token_lookahead_index = lookahead_index_backup;
            value = Parser_ParseAffect(parser);
            if (value) {
                node->as_statement.is_return_expr = false;
                node->as_statement.is_local_return = false;
                node->as_statement.is_mod_statement = true;
                node->as_statement.value = value;
            } else if (Parser_GetStatus(parser) == PARSER_OK) {
                // no affect so rollback lookahead index
                parser->token_lookahead_index = lookahead_index_backup;
                value = Parser_ParseExpr(parser);
                if (value) {
                    node->as_statement.is_return_expr = true;
                    node->as_statement.is_local_return = true;
                    node->as_statement.is_mod_statement = false;
                    node->as_statement.value = value;

                    tok = Parser_NextToken(parser, false, false);
                    if (tok && tok->type == TOKTYPE_SEMICOLON) {
                        node->as_statement.is_mod_statement = true;
                        node->as_statement.is_return_expr = false;
                        node->as_statement.is_local_return = false;
                    } else if (tok) {
                        Parser_PushBackTokenList(parser);
                    }
                } else if (Parser_GetStatus(parser) == PARSER_OK) {
                    nothing_to_parse = true;
                }
            }
        }
    }

    if (nothing_to_parse || Parser_GetStatus(parser) == PARSER_ERROR) {
        ASTNode_Free(node);
        node = NULL;
    } else if (!node->as_statement.is_mod_statement && parser->module_mode && module_scope) {
        loc = Parser_CurrentLocation(parser);
        parser->error = Err_NewWithLocation(
                "Return statements cannot be used outside of blocks in modules", loc);
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseArrayAccess(parser_t * parser) {
    ast_node_t * node, * expr;
    token_t * tok;
    loc_t loc;
    bool nothing_to_parse = false;

    node = ASTNode_New(NODE_ARRAY_ACCESS);

    tok = Parser_NextToken(parser, true, true);
    if (tok && tok->type == TOKTYPE_LSBRACKET) {
        expr = Parser_ParseExpr(parser);
        if (!expr) {
            if (!parser->error) {
                parser->status = PARSER_ERROR;
                loc = Parser_CurrentLocation(parser);
                parser->error = Err_NewWithLocation("Expected an expression after '['", loc);
            }
            ASTNode_Free(expr);
        } else {
            node->as_array_access.index_expr = expr;
            tok = Parser_NextToken(parser, true, true);
            if (!tok || tok->type != TOKTYPE_RSBRACKET) {
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    loc = Parser_CurrentLocation(parser);
                    parser->error = Err_NewWithLocation("Expected a ']' after index expression", loc);
                }
                if (tok) Parser_PushBackTokenList(parser);
            }
        }
    } else if (Parser_GetStatus(parser) == PARSER_OK) {
        if (tok) Parser_PushBackTokenList(parser);
        nothing_to_parse = true;
    }

    if (nothing_to_parse || Parser_GetStatus(parser) == PARSER_ERROR) {
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

/**
 * @retval an ast_node_t<ast_dotted_expr_t> * if there is atleast 2 components (a.b or a[b])
 * @retval an ast_node_t<ast_identifier_t> * if only one component
 */
ast_node_t * Parser_ParseDottedExpr(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_OBJ_FIELD_NAME,
        GOT_ARRAY_ACCESS,
        GOT_ARRAY_ACCESS2,
        GOT_DOT
    };
    enum {
        NONE,
        NO_IDENT,
        NO_OBJ_FIELD_NAME,
        ARRAY_ACCESS_ERROR
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;
    size_t lookahead_index;

    node = ASTNode_New(NODE_DOTTED_EXPR);

    do {
        switch (state) {
            case START:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_dotted_expr.components, value);
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT: // fallthrough
            case GOT_OBJ_FIELD_NAME:
                value = Parser_ParseArrayAccess(parser);
                if (!value) {
                    if (Parser_GetStatus(parser) == PARSER_ERROR) {
                        error_state = ARRAY_ACCESS_ERROR;
                    } else {
                        state = GOT_ARRAY_ACCESS2;
                    }
                } else {
                    Vec_Append(node->as_dotted_expr.components, value);
                    state = GOT_ARRAY_ACCESS;
                }
                break;
            
            case GOT_ARRAY_ACCESS:
                value = Parser_ParseArrayAccess(parser);
                if (!value) {
                    if (Parser_GetStatus(parser) == PARSER_ERROR) {
                        error_state = ARRAY_ACCESS_ERROR;
                    } else {
                        state = GOT_ARRAY_ACCESS2;
                    }
                } else {
                    Vec_Append(node->as_dotted_expr.components, value);
                    state = GOT_ARRAY_ACCESS;
                }
                break;
            
            case GOT_ARRAY_ACCESS2:
                lookahead_index = parser->token_lookahead_index; // fix for ease of parsing in Parser_ParseMsgPassExpr()
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_DOT) {
                    parser->token_lookahead_index = lookahead_index;
                    must_loop = false;
                    // if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_DOT;
                }
                break;
            
            case GOT_DOT:
                value = Parser_ParseObjFieldName(parser);
                if (!value) {
                    error_state = NO_OBJ_FIELD_NAME;
                } else {
                    Vec_Append(node->as_dotted_expr.components, value);
                    state = GOT_OBJ_FIELD_NAME;
                }
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
            case GOT_IDENT:
            case GOT_OBJ_FIELD_NAME:
            case GOT_ARRAY_ACCESS:
            case GOT_ARRAY_ACCESS2:
                break;
            
            case GOT_DOT:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a field name after the '.'", loc);
                }
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    } else if (Vec_GetLength(node->as_dotted_expr.components) == 1) {
        /* 
         * we shorten the ast tree by skipping nodes that were
         * the only children of their parent
         */
        ast_node_t * node2 = Vec_Pop(node->as_dotted_expr.components);
        ASTNode_Free(node);
        node = node2;
    }

    return node;
}

/**
 * Be sure to restore the lookahead index on return val = {NULL, NULL}
 */
ast_node_t * Parser_ParseAffect(parser_t * parser) {
    enum {
        START,
        GOT_DOTTED_EXPR,
        GOT_EQUAL,
        GOT_EXPR,
        GOT_SEMICOLON
    };
    enum {
        NONE,
        NO_DOTTED_EXPR,
        DOTTED_EXPR_ERROR,
        NO_EQUAL,
        NO_EXPR,
        EXPR_ERROR,
        NO_SEMICOLON
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_AFFECT);

    do {
        switch (state) {
            case START:
                value = Parser_ParseDottedExpr(parser);
                if (!value) {
                    error_state = Parser_GetStatus(parser) == PARSER_ERROR
                            ? DOTTED_EXPR_ERROR : NO_DOTTED_EXPR;
                } else {
                    node->as_affect.lval = value;
                    state = GOT_DOTTED_EXPR;
                }
                break;

            case GOT_DOTTED_EXPR:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_EQUAL) {
                    error_state = NO_EQUAL;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_EQUAL;
                }
                break;
            
            case GOT_EQUAL: // from here we know for sure it's an affect
                value = Parser_ParseExpr(parser);
                if (!value) {
                    error_state = Parser_GetStatus(parser) == PARSER_ERROR
                            ? EXPR_ERROR : NO_EXPR;
                } else {
                    node->as_affect.rval = value;
                    state = GOT_EXPR;
                }
                break;
            
            case GOT_EXPR:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_SEMICOLON) {
                    error_state = NO_SEMICOLON;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_SEMICOLON;
                }
                break;
            
            case GOT_SEMICOLON:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if ti needs to
            case GOT_DOTTED_EXPR: // we don't know for sure if it's an affect
            case GOT_SEMICOLON:
                break;
            
            case GOT_EQUAL: // with an '=' supplied we assume that's an affect
                            // so we set 'error' if not already set
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected an expression after '='", loc);
                }
                break;
            
            case GOT_EXPR:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a trailing ';'", loc);
                }
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseUnaryExpr(parser_t * parser) {
    ast_node_t * node, * value;
    token_t * tok;
    bool nothing_to_parse = false;

    node = ASTNode_New(NODE_UNARY_EXPR);
    tok = Parser_NextToken(parser, false, false);
    if (tok) {
        if (tok->type == TOKTYPE_EXCL
                || tok->type == TOKTYPE_PLUS
                || tok->type == TOKTYPE_MINUS) {
            node->as_expr.op = tok->type;
            value = Parser_ParseAtomExpr(parser);
            if (!value) {
                if (Parser_GetStatus(parser) == PARSER_OK) {
                    nothing_to_parse = true;
                }
            } else {
                Vec_Append(node->as_expr.values, value);
            }
        } else {
            nothing_to_parse = true;
        }
    }
    
    if (!tok || Parser_GetStatus(parser) == PARSER_ERROR || nothing_to_parse) {
        if (tok) Parser_PushBackTokenList(parser);
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseAtomExpr(parser_t * parser) {
    ast_node_t * node = NULL, * value;
    token_t * tok;
    ast_node_t * (*funcs[])(parser_t *) = {
        Parser_ParseDottedExpr, Parser_ParseUnaryExpr, Parser_ParseLitteralExpr
    };

    for (size_t i = 0; i < 3; i++) {
        value = funcs[i](parser);
        if (value || Parser_GetStatus(parser) == PARSER_ERROR) return value;
    }

    tok = Parser_NextToken(parser, false, false);
    if (tok) {
        if (tok->type == TOKTYPE_LPAREN) {
            value = Parser_ParseExpr(parser);
            if (value) {
                tok = Parser_NextToken(parser, false, false);
                if (tok && tok->type == TOKTYPE_RPAREN) {
                    node = value;
                } else {
                    if (!parser->error) {
                        parser->status = PARSER_ERROR;
                        loc_t loc = Parser_CurrentLocation(parser);
                        parser->error = Err_NewWithLocation("Expected a ')' after expression", loc);
                    }
                    ASTNode_Free(value);
                }
            } else if (!parser->error) {
                parser->status = PARSER_ERROR;
                loc_t loc = Parser_CurrentLocation(parser);
                parser->error = Err_NewWithLocation("Expected an expression after '('", loc);
            }
        } else {
            Parser_PushBackTokenList(parser);
        }
    }

    return node;
}

ast_node_t * Parser_ParseLitteralExpr(parser_t * parser) {
    ast_node_t * value;
    ast_node_t * (*funcs[])(parser_t *) = {
        Parser_ParseInt, Parser_ParseDouble, Parser_ParseString,
        Parser_ParseArrayLitteral, Parser_ParseObjLitteral, Parser_ParseBlock
    };

    for (size_t i = 0; i < 5; i++) {
        value = funcs[i](parser);
        if (value || Parser_GetStatus(parser) == PARSER_ERROR) return value;
    }
    return value;
}

ast_node_t * Parser_ParseArrayLitteral(parser_t * parser) {
    enum {
        START,
        GOT_LSBRACKET,
        GOT_EXPR,
        GOT_COMMA,
        GOT_RSBRACKET
    };
    enum {
        NONE,
        NO_LSBRACKET,
        EXPR_ERROR,
        NO_RSBRACKET,
        GOT_COMMA_BUT_NO_EXPR
    };
    ast_node_t * node;
    ast_node_t * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_ARRAY_LITTERAL);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_LSBRACKET) {
                    error_state = NO_LSBRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_LSBRACKET;
                }
                break;
            
            case GOT_COMMA:
            case GOT_LSBRACKET:
                value = Parser_ParseExpr(parser);
                if (value) {
                    Vec_Append(node->as_array_litteral.items, value);
                    state = GOT_EXPR;
                } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
                    error_state = EXPR_ERROR;
                } else {
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_RSBRACKET) {
                        if (Vec_GetLength(node->as_array_litteral.items) == 0
                                && tok && tok->type == TOKTYPE_COMMA) {
                            error_state = GOT_COMMA_BUT_NO_EXPR;
                        } else {
                            error_state = NO_RSBRACKET;
                        }
                        if (tok) Parser_PushBackTokenList(parser);
                    } else {
                        state = GOT_RSBRACKET;
                    }
                }
                break;
            
            case GOT_EXPR:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || (tok->type != TOKTYPE_COMMA
                        && tok->type != TOKTYPE_RSBRACKET)) {
                    error_state = NO_RSBRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = tok->type == TOKTYPE_COMMA ? GOT_COMMA : GOT_RSBRACKET;
                }
                break;
            
            case GOT_RSBRACKET:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
            case GOT_RSBRACKET:
                break;
            
            case GOT_EXPR:
            case GOT_COMMA:
            case GOT_LSBRACKET: // for error_state == EXPR_ERROR 'error' is already set
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    if (error_state == NO_RSBRACKET) {
                        parser->error = Err_NewWithLocation("Expected a ']' to close the array", loc);
                    } else if (error_state == GOT_COMMA_BUT_NO_EXPR) {
                        parser->error = Err_NewWithLocation("Expected at least one expression before a ','", loc);
                    }
                }
                break;
            
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseBlock(parser_t * parser) {
    enum {
        START,
        GOT_LCBRACKET,
        GOT_PIPE,
        GOT_IDENT,
        PARSE_STATEMENT,
        GOT_STATEMENT,
        GOT_RCBRACKET
    };
    enum {
        NONE,
        NO_LCBRACKET,
        NO_IDENT,
        NO_PIPE,
        NO_STATEMENT,
        NO_RCBRACKET
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;
    size_t  lookahead_index = parser->token_lookahead_index;

    node = ASTNode_New(NODE_BLOCK);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_LCBRACKET) {
                    error_state = NO_LCBRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_LCBRACKET;
                }
                break;
            
            case GOT_LCBRACKET:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_PIPE) {
                    state = PARSE_STATEMENT;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_PIPE;
                }
                break;
            
            case GOT_PIPE:
                value = Parser_ParseIdentifier(parser, false);
                if (value) {
                    Vec_Append(node->as_block.params, value);
                    state = GOT_IDENT;
                } else {
                    error_state = NO_IDENT;
                }
                break;
            
            case GOT_IDENT:
                value = Parser_ParseIdentifier(parser, false);
                if (value) {
                    Vec_Append(node->as_block.params, value);
                    state = GOT_IDENT;
                } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
                    error_state = NO_IDENT;
                } else {
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_PIPE) {
                        error_state = NO_PIPE;
                        if (tok) Parser_PushBackTokenList(parser);
                    } else {
                        state = GOT_STATEMENT;
                    }
                }
                break;
            
            case PARSE_STATEMENT:
                value = Parser_ParseStatement(parser, false);
                if (value) {
                    Vec_Append(node->as_block.statements, value);
                    state = GOT_STATEMENT;
                } else {
                    error_state = NO_STATEMENT;
                }
                break;
            
            case GOT_STATEMENT:
                value = Parser_ParseStatement(parser, false);
                if (value) {
                    Vec_Append(node->as_block.statements, value);
                    state = GOT_STATEMENT;
                } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
                    error_state = NO_STATEMENT;
                } else {
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_RCBRACKET) {
                        error_state = NO_RCBRACKET;
                        if (tok) Parser_PushBackTokenList(parser);
                    } else {
                        state = GOT_RCBRACKET;
                    }
                }
                break;
            
            case GOT_RCBRACKET:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
            case GOT_LCBRACKET:
            case GOT_RCBRACKET:
                break;
            
            case GOT_PIPE:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a block parameter name after '|'", loc);
                }
                break;
            
            case GOT_IDENT:
                if (!parser->error && error_state == NO_PIPE) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a '|' after block parameters names", loc);
                }
                break;

            case PARSE_STATEMENT: // if no statement and no param then it's an object litteral
                parser->token_lookahead_index = lookahead_index;
                break;
            
            case GOT_STATEMENT:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a '}' at end of block", loc);
                }
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

/**
 * @returns {NULL, NULL} when nothing to parse or when suspecting the presence
 *          of an obj_field_init
 */
ast_node_t * Parser_ParseObjMsgDef(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_COLON,
        GOT_IDENT2,
        GOT_IDENT3,
        GOT_COLON2,
        GOT_IDENT4,
        GOT_LCBRACKET,
        GOT_STATEMENT,
        GOT_RCBRACKET
    };
    enum {
        NONE,
        NO_IDENT,
        NO_COLON_OR_BRACKET,
        NO_COLON,
        NO_LCBRACKET,
        NO_RCBRACKET,
        STATEMENT_ERROR
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;
    size_t lookahead_index = parser->token_lookahead_index;

    node = ASTNode_New(NODE_OBJ_MSG_DEF);

    do {
        switch (state) {
            case START:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_obj_msg_def.selector, value);
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || (tok->type != TOKTYPE_COLON 
                        && tok->type != TOKTYPE_LCBRACKET)) {
                    error_state = NO_COLON_OR_BRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = tok->type == TOKTYPE_COLON ? GOT_COLON : GOT_LCBRACKET;
                }
                break;
            
            case GOT_COLON:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_obj_msg_def.selector, value);
                    state = GOT_IDENT2;
                }
                break;
            
            case GOT_IDENT2:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    // the message has one parameter
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_LCBRACKET) {
                        error_state = NO_LCBRACKET;
                        if (tok) Parser_PushBackTokenList(parser);
                    } else {
                        state = GOT_LCBRACKET;
                    }
                } else {
                    Vec_Append(node->as_obj_msg_def.selector, value);
                    state = GOT_IDENT3;
                }
                break;
            
            case GOT_IDENT3:
                tok = Parser_NextToken(parser, true, true);
                if (!tok || tok->type != TOKTYPE_COLON) {
                    // error
                    error_state = NO_COLON;
                    if (tok) Parser_PushBackTokenList(parser);
                } else if (tok->type == TOKTYPE_COLON) {
                    state = GOT_COLON2;
                }
                break;
            
            case GOT_COLON2:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_obj_msg_def.selector, value);
                    state = GOT_IDENT4;
                }
                break;
            
            case GOT_IDENT4:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    // the message has no more parameters
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_LCBRACKET) {
                        error_state = NO_LCBRACKET;
                    } else {
                        state = GOT_LCBRACKET;
                    }
                } else {
                    Vec_Append(node->as_obj_msg_def.selector, value);
                    state = GOT_IDENT3;
                }
                break;
            
            case GOT_LCBRACKET:
            case GOT_STATEMENT:
                value = Parser_ParseStatement(parser, false);
                if (!value) {
                    // the block contains no statements
                    tok = Parser_NextToken(parser, false, false);
                    if (!tok || tok->type != TOKTYPE_RCBRACKET) {
                        error_state = NO_RCBRACKET;
                    } else {
                        state = GOT_RCBRACKET;
                    }
                } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
                    error_state = STATEMENT_ERROR;
                } else {
                    state = GOT_STATEMENT;
                    Vec_Append(node->as_obj_msg_def.statements, value);
                }
                break;
            
            case GOT_RCBRACKET:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // parser->error is already set if it needs to
                break;
            
            case GOT_COLON2:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected another parameter name after ':'", loc);
                }
                break;
            
            case GOT_COLON:
            case GOT_IDENT2:
            case GOT_IDENT3:
                // we are not sure, maybe it's an obj_field_init
                parser->token_lookahead_index = lookahead_index;
                break;

            case GOT_IDENT:
            case GOT_IDENT4:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a '{' after the message signature", loc);
                }
                break;

            case GOT_LCBRACKET:
            case GOT_STATEMENT:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a '}' after the message signature", loc);
                }
                break;

            case GOT_RCBRACKET: // terminal state
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseObjFieldInit(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_COLON,
        GOT_EXPRESSION
    };
    enum {
        NONE,
        NO_IDENT,
        NO_COLON,
        NO_EXPRESSION,
        EXPR_ERROR
    };
    ast_node_t * node, * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_OBJ_FIELD_INIT);

    do {
        switch (state) {
            case START:
                value = Parser_ParseIdentifier(parser, false);
                if (!value) {
                    error_state = NO_IDENT;
                } else {
                    node->as_obj_field_init.ident = value;
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT:
                tok = Parser_NextToken(parser, true, true);
                if (!tok || tok->type != TOKTYPE_COLON) {
                    error_state = NO_COLON;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_COLON;
                }
                break;
            
            case GOT_COLON:
                value = Parser_ParseExpr(parser);
                if (!value) {
                    error_state = NO_EXPRESSION;
                } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
                    error_state = EXPR_ERROR;
                } else {
                    node->as_obj_field_init.value = value;
                    state = GOT_EXPRESSION;
                }
                break;
            
            case GOT_EXPRESSION:
                must_loop = false;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // no message definition can be parsed, it's ok
                break;

            case GOT_IDENT:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a ':' after identifier", loc);
                }
                break;
            
            case GOT_COLON:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected an expression after ':'", loc);
                }
                break;
            
            case GOT_EXPRESSION: // terminal state
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

ast_node_t * Parser_ParseObjLitteral(parser_t * parser) {
    enum {
        START,
        GOT_LCBRACKET,
        GOT_MEMBER,
        GOT_COMMA,
        GOT_RCBRACKET
    };
    enum {
        NONE,
        NO_LCBRACKET,
        MEMBER_ERROR,
        NO_COMMA_OR_RCBRACKET,
        NO_RCBRACKET
    };
    ast_node_t * node;
    ast_node_t * value;
    token_t * tok;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_OBJ_LITTERAL);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_LCBRACKET) {
                    error_state = NO_LCBRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_LCBRACKET;
                }
                break;
            
            case GOT_LCBRACKET:
            case GOT_COMMA:
                value = Parser_ParseObjMsgDef(parser);
                if (value) {
                    Vec_Append(node->as_obj_litteral.obj_fields, value);
                    state = GOT_MEMBER;
                } else if (Parser_GetStatus(parser) == PARSER_OK) {
                    value = Parser_ParseObjFieldInit(parser);
                    if (value) {
                        Vec_Append(node->as_obj_litteral.obj_fields, value);
                        state = GOT_MEMBER;
                    } else if (Parser_GetStatus(parser) == PARSER_OK) {
                        tok = Parser_NextToken(parser, false, false);
                        if (!tok || tok->type != TOKTYPE_RCBRACKET) {
                            error_state = NO_RCBRACKET;
                            if (tok) Parser_PushBackTokenList(parser);
                        } else {
                            state = GOT_RCBRACKET;
                        }
                    }
                }
                break;
            
            case GOT_MEMBER:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || (tok->type != TOKTYPE_COMMA
                        && tok->type != TOKTYPE_RCBRACKET)) {
                    error_state = NO_COMMA_OR_RCBRACKET;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = tok->type == TOKTYPE_COMMA ? GOT_COMMA: GOT_RCBRACKET;
                }
                break;
            
            case GOT_RCBRACKET:
                must_loop = false;
                break;

        }
    } while (must_loop && !error_state);

    if (error_state || Parser_GetStatus(parser) == PARSER_ERROR) {
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // no obj_litteral to parse, it's ok
                break;
            
            case GOT_LCBRACKET:
            case GOT_COMMA:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected a '}' at the end of the object litteral", loc);
                }
                break;
            
            case GOT_MEMBER:
                if (!parser->error) {
                    parser->status = PARSER_ERROR;
                    parser->error = Err_NewWithLocation("Expected ',' to add another member "
                                                        "or '}' to close the object litteral", loc);
                }
                break;
            
            case GOT_RCBRACKET: // terminal state
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}
