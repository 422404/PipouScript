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
    bool must_loop;

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
ast_node_t * Parser_CreateAST(parser_t * parser) {
    /// @todo test
    return Parser_ParseObjFieldName(parser);
}

ast_node_t * Parser_ParseIdentifier(parser_t * parser, bool directly) {
    token_t * token;
    ast_node_t * node = NULL;
    
    token = Parser_NextToken(parser, !directly, !directly);
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
        NO_IDENT,
    };
    ast_node_t * node, * ident;
    token_t * tok;
    int state = START;
    int error = NONE;
    bool must_loop = true;

    node = ASTNode_New(NODE_OBJ_FIELD_NAME);

    do {
        switch (state) {
            case START:
                tok = Parser_NextToken(parser, false, false);
                if (!tok) {
                    // error
                    error = NO_TOKEN;
                    break;
                }
                if (tok->type == TOKTYPE_HASH) {
                    node->as_obj_field_name.is_msg_name = true;
                    state = GOT_HASH;
                } else {
                    Parser_PushBackTokenList(parser);
                    ident = Parser_ParseIdentifier(parser, false);
                    if (!ident) {
                        // error
                        error = NO_IDENT;
                        break;
                    }
                    Vec_Append(node->as_obj_field_name.components, ident);
                    state = GOT_IDENT;
                }
                break;
            
            case GOT_IDENT:
                must_loop = false;
                break;
            
            case GOT_HASH:
            case GOT_COLON:
                ident = Parser_ParseIdentifier(parser, true);
                if (!ident) {
                    // error
                    error = NO_IDENT;
                    break;
                }
                Vec_Append(node->as_obj_field_name.components, ident);
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
    } while (must_loop && !error);

    if (error) {
        char buf[256];
        loc_t loc = Parser_CurrentLocation(parser);
        bool is_hash = false;
        switch (state) {
            case START:
                snprintf(buf, 256, error == NO_TOKEN
                        ? "End of token stream"
                        : "Expected an identifier");
                Err_SetError(Err_NewWithLocation(buf, loc));
                break;
            
            case GOT_HASH:
                is_hash = true; // fallthrough
            case GOT_COLON:
                snprintf(buf, 256, is_hash
                        ? "Expected a parameter name after '#'"
                        : "Expected another parameter name after ':'");
                Err_SetError(Err_NewWithLocation(buf, loc));
                break;
            
            // terminal nodes so no errors possible
            case GOT_IDENT:
            case GOT_IDENT2:
                break;
        }
        ASTNode_Free(node);
        node = NULL;
    }

    return node;
}

/*
ast_node_t * Parser_ParseArrayLitteral(parser_t * parser);
ast_node_t * Parser_ParseBlock(parser_t * parser);
ast_node_t * Parser_ParseArrayAccess(parser_t * parser);
ast_node_t * Parser_ParseDottedExpr(parser_t * parser);
ast_node_t * Parser_ParseMsgPassExpr(parser_t * parser);
ast_node_t * Parser_ParseExpr(parser_t * parser, ast_node_type_t type);
ast_node_t * Parser_ParseStatement(parser_t * parser);
*/
