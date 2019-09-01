/**
 * @file parser.c
 * Parser implementation
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parser/include/parser.h"
#include "error.h"
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

/**
 * @todo Test it
 */
static loc_t Parser_CurrentLocation(parser_t * parser) {
    loc_t loc;

    if (parser->token_lookahead_index < Vec_GetLength(parser->token_lookahead)) {
        loc = ((token_t *)Vec_GetAt(parser->token_lookahead, parser->token_lookahead_index))->span.end;
    } else {
        /// @todo Lex_GetPos()
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
        Parser_ConsumeLookahead(parser);
        Vec_Free(parser->token_lookahead);
        free(parser);
    }
    else Err_Throw(Err_New("NULL pointer to parser"));
}

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser The parser used to generate the AST
 * @returns          The AST root node
 */
parse_result_t Parser_CreateAST(parser_t * parser) {
    /// @todo test
    return Parser_ParseDottedExpr(parser);
}

parse_result_t Parser_ParseIdentifier(parser_t * parser, bool directly) {
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

    parse_result_t res = {node, NULL};
    return res;
}

parse_result_t Parser_ParseString(parser_t * parser) {
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

    parse_result_t res = {node, NULL};
    return res;
}

parse_result_t Parser_ParseInt(parser_t * parser) {
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

    parse_result_t res = {node, NULL};
    return res;
}

parse_result_t Parser_ParseDouble(parser_t * parser) {
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

    parse_result_t res = {node, NULL};
    return res;
}

parse_result_t Parser_ParseObjFieldName(parser_t * parser) {
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
        NO_IDENT,
    };
    ast_node_t * node;
    parse_result_t ident;
    token_t * tok;
    error_t * error = NULL;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_OBJ_FIELD_NAME);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok) {
                    // error
                    error_state = NO_TOKEN;
                    break;
                }
                if (tok->type == TOKTYPE_HASH) {
                    node->as_obj_field_name.is_msg_name = true;
                    state = GOT_HASH;
                } else {
                    Parser_PushBackTokenList(parser);
                    ident = Parser_ParseIdentifier(parser, false);
                    if (!ident.node) {
                        // error
                        error_state = NO_IDENT;
                        break;
                    }
                    Vec_Append(node->as_obj_field_name.components, ident.node);
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT:
                must_loop = false;
                break;
            
            case GOT_HASH:
            case GOT_COLON:
                ident = Parser_ParseIdentifier(parser, true);
                if (!ident.node) {
                    // error
                    error_state = NO_IDENT;
                    break;
                }
                Vec_Append(node->as_obj_field_name.components, ident.node);
                state = GOT_IDENT2;
                break;
            
            case GOT_IDENT2:
                tok = Parser_NextToken(parser, true, true);
                if (tok && tok->type == TOKTYPE_COLON) {
                    state = GOT_COLON;
                } else {
                    must_loop = false;
                    Parser_PushBackTokenList(parser);
                }
                break;

        }
    } while (must_loop && !error_state);

    if (error_state) {
        char buf[256];
        loc_t loc = Parser_CurrentLocation(parser);
        bool is_hash = false;
        switch (state) {
            // we found nothing, we let error == NULL to indicate it
            case START:
                break;
            
            case GOT_HASH:
                is_hash = true; // fallthrough
            case GOT_COLON:
                snprintf(buf, 256, is_hash
                        ? "Expected a parameter name after '#'"
                        : "Expected another parameter name after ':'");
                error = Err_NewWithLocation(buf, loc);
                break;
            
            // terminal states so no errors possible
            case GOT_IDENT:
            case GOT_IDENT2:
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

parse_result_t Parser_ParseMsgSel(parser_t * parser) {
    enum {
        START,
        GOT_IDENT,
        GOT_COLON,
        GOT_IDENT2,
        GOT_IDENT3,
        GOT_COLON2,
        GOT_IDENT4
    };
    enum {
        NONE,
        NO_IDENT,
        NO_COLON
    };
    ast_node_t * node;
    parse_result_t ident;
    token_t * tok;
    error_t * error = NULL;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_MSG_SEL);

    /** @todo eliminate code repetition */
    do {
        switch (state) {
            case START:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    error_state = NO_IDENT;
                    break;
                }
                Vec_Append(node->as_msg_sel.ident_list, ident.node);
                state = GOT_IDENT;
                break;
            
            case GOT_IDENT:
                tok = Parser_NextToken(parser, true, true);
                if (!tok || tok->type != TOKTYPE_COLON) {
                    // the message selector is only composed of a name
                    must_loop = false;
                    if (tok) Parser_PushBackTokenList(parser);
                } else if (tok->type == TOKTYPE_COLON) {
                    state = GOT_COLON;
                }
                break;
            
            case GOT_COLON:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    error_state = NO_IDENT;
                    break;
                }
                Vec_Append(node->as_msg_sel.ident_list, ident.node);
                state = GOT_IDENT2;
                break;
            
            case GOT_IDENT2:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    // the message has one parameter
                    must_loop = false;
                    break;
                }
                Vec_Append(node->as_msg_sel.ident_list, ident.node);
                state = GOT_IDENT3;
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
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    error_state = NO_IDENT;
                    break;
                }
                Vec_Append(node->as_msg_sel.ident_list, ident.node);
                state = GOT_IDENT4;
                break;
            
            case GOT_IDENT4:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    // the message has no more parameters
                    must_loop = false;
                    break;
                }
                Vec_Append(node->as_msg_sel.ident_list, ident.node);
                state = GOT_IDENT3;
                break;
        }
    } while (must_loop && !error_state);

    if (error_state) {
        char buf[256];
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // no message selector can be parsed, it's ok
                break;
            
            case GOT_COLON:
            case GOT_COLON2:
                snprintf(buf, 256, "Expected another parameter name after ':'");
                error = Err_NewWithLocation(buf, loc);
                break;
            
            case GOT_IDENT3:
                snprintf(buf, 256, "Expected a ':' after identifier \"%s\"", ident.node->as_ident.value);
                error = Err_NewWithLocation(buf, loc);
                break;
            
            // terminal states, no errors possible
            case GOT_IDENT:
            case GOT_IDENT2:
            case GOT_IDENT4:
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

/**
 * @todo make it parse things other than identifier
 */
parse_result_t Parser_ParseExpr(parser_t * parser, ast_node_type_t type) {
    ast_node_t * node;
    parse_result_t ident;
    
    node = ASTNode_New(type);
    ident = Parser_ParseIdentifier(parser, false);
    if (ident.node) {
        Vec_Append(node->as_expr.values, ident.node);
    } else {
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, NULL};
    return res;
}

parse_result_t Parser_ParseDecl(parser_t * parser) {
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
    ast_node_t * node;
    parse_result_t ident, expr;
    token_t * tok;
    error_t * error = NULL;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_DECL);

    /** @todo eliminate code repetition */
    do {
        switch (state) {
            case START:
                ident = Parser_ParseIdentifier(parser, false);
                if (!ident.node) {
                    error_state = NO_IDENT;
                } else {
                    node->as_decl.lval = ident.node;
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
            
            case GOT_COLEQUAL:
                expr = Parser_ParseExpr(parser, NODE_OR_EXPR);
                if (!expr.node) {
                    error_state = NO_EXPR;
                    error = expr.error;
                } else {
                    node->as_decl.rval = expr.node;
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

    if (error_state) {
        char buf[256];
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // no decl to parse, it's ok
                break;

            case GOT_IDENT: // no COLEQUAL has been found
                snprintf(buf, 256, "Expected a ':=' after identifier \"%s\"", ident.node->as_ident.value);
                error = Err_NewWithLocation(buf, loc);
                break;
            
            case GOT_COLEQUAL: // no expr has been found
                if (!error) {
                    snprintf(buf, 256, "Expected an expression after ':='");
                    error = Err_NewWithLocation(buf, loc);
                }
                break;
            
            case GOT_EXPR: // no SEMICOLON has been found
                snprintf(buf, 256, "Expected a ';' at the end of the declaration statement");
                error = Err_NewWithLocation(buf, loc);
                break;
            
            case GOT_SEMICOLON: // terminal state, no errors possible
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

/**
 * @todo tests
 * @todo iterate over statements when they are not module statement but we are in module mode
 */
parse_result_t Parser_ParseStatement(parser_t * parser) {
    ast_node_t * node;
    parse_result_t value;
    token_t * tok;
    char buf[256];
    error_t * error = NULL;
    loc_t loc;
    bool nothing_to_parse = false;

    node = ASTNode_New(NODE_STATEMENT);

    tok = Parser_NextToken(parser, false, false);
    if (tok && tok->type == TOKTYPE_CIRCUMFLEX) {
        node->as_statement.is_return_expr = true;
        node->as_statement.is_local_return = false;
        node->as_statement.is_mod_statement = false;
        value = Parser_ParseExpr(parser, NODE_OR_EXPR);
        if (!value.node) {
            loc = Parser_CurrentLocation(parser);
            snprintf(buf, 256, "Expected an identifier after '^'");
            error = Err_NewWithLocation(buf, loc);
        } else {
            node->as_statement.value = value.node;
        }
    } else {
        // try to parse the complex ones first (decl and affect)
        value = Parser_ParseDecl(parser);
        if (value.node) {
            node->as_statement.is_return_expr = false;
            node->as_statement.is_local_return = false;
            node->as_statement.is_mod_statement = true;
            node->as_statement.value = value.node;
        } else if (value.error) {
            error = value.error;
        } else {
            /*
            // save the index of the token before so we can rollback 
            // -> todo into Parser_ParseAffect()
            value = Parser_ParseAffect(parser);
            if (value.node) {
                node->as_statement.is_return_expr = false;
                node->as_statement.is_local_return = false;
                node->as_statement.is_mod_statement = true;
                node->as_statement.value = value.node;
            } else if (value.error) {
                error = value.error;
            } else {*/
                value = Parser_ParseExpr(parser, NODE_OR_EXPR);
                if (value.node) {
                    node->as_statement.is_return_expr = true;
                    node->as_statement.is_local_return = true;
                    node->as_statement.value = value.node;

                    tok = Parser_NextToken(parser, false, false);
                    node->as_statement.is_mod_statement = tok && tok->type == TOKTYPE_SEMICOLON;
                    if (!node->as_statement.is_mod_statement && tok) {
                        Parser_PushBackTokenList(parser);
                    }
                } else if (value.error) {
                    error = value.error;
                } else {
                    nothing_to_parse = true;
                }
            //}
        }
    }

    if (nothing_to_parse || error) {
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

parse_result_t Parser_ParseArrayAccess(parser_t * parser) {
    ast_node_t * node;
    parse_result_t expr;
    token_t * tok;
    error_t * error = NULL;
    char buf[256];
    loc_t loc;
    bool nothing_to_parse = false;

    node = ASTNode_New(NODE_ARRAY_ACCESS);

    tok = Parser_NextToken(parser, true, true);
    if (tok && tok->type == TOKTYPE_LSBRACKET) {
        expr = Parser_ParseExpr(parser, NODE_OR_EXPR);
        if (!expr.node) {
            if (expr.error) error = expr.error;
            else {
                loc = Parser_CurrentLocation(parser);
                snprintf(buf, 256, "Expected an expression after '['");
                error = Err_NewWithLocation(buf, loc);
            }
            ASTNode_Free(expr.node);
        } else {
            node->as_array_access.index_expr = expr.node;
            tok = Parser_NextToken(parser, true, true);
            if (!tok || tok->type != TOKTYPE_RSBRACKET) {
                loc = Parser_CurrentLocation(parser);
                snprintf(buf, 256, "Expected a ']' after index expression");
                error = Err_NewWithLocation(buf, loc);
                if (tok) Parser_PushBackTokenList(parser);
            }
        }
    } else {
        if (tok) Parser_PushBackTokenList(parser);
        nothing_to_parse = true;
    }

    if (nothing_to_parse || error) {
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

parse_result_t Parser_ParseDottedExpr(parser_t * parser) {
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
    ast_node_t * node;
    parse_result_t value;
    token_t * tok;
    error_t * error = NULL;
    int state = START;
    int error_state = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_DOTTED_EXPR);

    do {
        switch (state) {
            case START:
                value = Parser_ParseIdentifier(parser, false);
                if (!value.node) {
                    error_state = NO_IDENT;
                } else {
                    Vec_Append(node->as_dotted_expr.components, value.node);
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT: // fallthrough
            case GOT_OBJ_FIELD_NAME:
                value = Parser_ParseArrayAccess(parser);
                if (!value.node) {
                    if (value.error) {
                        error = value.error;
                        error_state = ARRAY_ACCESS_ERROR;
                    } else {
                        state = GOT_ARRAY_ACCESS;
                    }
                } else {
                    Vec_Append(node->as_dotted_expr.components, value.node);
                    state = GOT_ARRAY_ACCESS;
                }
                break;
            
            case GOT_ARRAY_ACCESS:
                value = Parser_ParseArrayAccess(parser);
                if (!value.node) {
                    if (value.error) {
                        error = value.error;
                        error_state = ARRAY_ACCESS_ERROR;
                    } else {
                        state = GOT_ARRAY_ACCESS2;
                    }
                } else {
                    Vec_Append(node->as_dotted_expr.components, value.node);
                    state = GOT_ARRAY_ACCESS;
                }
                break;
            
            case GOT_ARRAY_ACCESS2:
                tok = Parser_NextToken(parser, false, false);
                if (!tok || tok->type != TOKTYPE_DOT) {
                    must_loop = false;
                    if (tok) Parser_PushBackTokenList(parser);
                } else {
                    state = GOT_DOT;
                }
                break;
            
            case GOT_DOT:
                value = Parser_ParseObjFieldName(parser);
                if (!value.node) {
                    error_state = NO_OBJ_FIELD_NAME;
                    if (value.error) error = value.error;
                } else {
                    Vec_Append(node->as_dotted_expr.components, value.node);
                    state = GOT_OBJ_FIELD_NAME;
                }
                break;
        }
    } while (must_loop && !error_state);

    if (error_state) {
        char buf[256];
        loc_t loc = Parser_CurrentLocation(parser);
        switch (state) {
            case START: // no identifier to parse, it's ok
                break;

            case GOT_IDENT:          // fallthrough
            case GOT_OBJ_FIELD_NAME: // fallthrough
            case GOT_ARRAY_ACCESS:   // 'error' is already set
                break;
            
            case GOT_DOT:
                if (!error) {
                    snprintf(buf, 256, "Expected a field name after the '.'");
                    error = Err_NewWithLocation(buf, loc);
                }
                break;

            case GOT_ARRAY_ACCESS2: // terminal state so no errors possible
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    parse_result_t res = {node, error};
    return res;
}

/*
parse_result_t Parser_ParseAffect(parser_t * parser);
parse_result_t Parser_ParseObjFieldInit(parser_t * parser);
parse_result_t Parser_ParseObjMsgDef(parser_t * parser);
parse_result_t Parser_ParseObjLitteral(parser_t * parser);
parse_result_t Parser_ParseArrayLitteral(parser_t * parser);
parse_result_t Parser_ParseBlock(parser_t * parser);
parse_result_t Parser_ParseMsgPassExpr(parser_t * parser);
parse_result_t Parser_ParseExpr(parser_t * parser, ast_node_type_t type);
*/
