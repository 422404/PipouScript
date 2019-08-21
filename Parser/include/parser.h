/**
 * @file parser.h
 * Parser implementation
 */
#pragma once
#include <stdbool.h>
#include "lexer.h"
#include "vector.h"
#include "ast.h"

typedef struct {
    /**
     * When true the file is parsed for "statement" grammar's rules
     * When false the file is parsed for "mod_statement" grammar's rules
     */
    bool module_mode;

    /** File that contains the code */
    char * filename;

    /** The lexer the token stream is read from */
    lexer_t * lexer;

    /**
     * Token list used to provide some lookahead in the lexer stream
     * vector_t<token_t *>
     */
    vector_t * token_lookahead;

    /** Index in token list */
    size_t token_lookahead_index;
} parser_t;


/**
 * Allocates a new parser
 * @param[in] buffer      The buffer where the code to be parsed is located
 * @param     length      Length of the buffer
 * @param[in] filename    Name of the file that contains the code
 * @param     module_mode Indicates if the code resides in a module
 *                        or in the main source file
 * @returns               The newly allocated parser
 */
parser_t * Parser_New(char * buffer, size_t length, char * filename, bool module_mode);

/**
 * Frees a previously allocated parser
 * @param[in] parser The parser to be freed
 */
void Parser_Free(parser_t * parser);

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser The parser used to generate the AST
 * @returns          The AST root node
 */
ast_node_t * Parser_CreateAST(parser_t * parser);

/**
 * @todo doc
 * @param[in] parser
 * @param     directly Parse directly on the next token without skipping whitespaces
 *                     or comments 
 */
ast_node_t * Parser_ParseIdentifier(parser_t * parser, bool direct);
ast_node_t * Parser_ParseString(parser_t * parser);
ast_node_t * Parser_ParseInt(parser_t * parser);
ast_node_t * Parser_ParseDouble(parser_t * parser);

ast_node_t * Parser_ParseDecl(parser_t * parser);
ast_node_t * Parser_ParseAffect(parser_t * parser);
ast_node_t * Parser_ParseObjFieldInit(parser_t * parser);
ast_node_t * Parser_ParseMsgSel(parser_t * parser);
ast_node_t * Parser_ParseObjMsgDef(parser_t * parser);
ast_node_t * Parser_ParseObjLitteral(parser_t * parser);
ast_node_t * Parser_ParseObjFieldName(parser_t * parser);
ast_node_t * Parser_ParseArrayLitteral(parser_t * parser);
ast_node_t * Parser_ParseBlock(parser_t * parser);
ast_node_t * Parser_ParseArrayAccess(parser_t * parser);
ast_node_t * Parser_ParseDottedExpr(parser_t * parser);
ast_node_t * Parser_ParseMsgPassExpr(parser_t * parser);
ast_node_t * Parser_ParseExpr(parser_t * parser, ast_node_type_t type);
ast_node_t * Parser_ParseStatement(parser_t * parser);
