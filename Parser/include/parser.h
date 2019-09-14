/**
 * @file parser.h
 * Parser implementation
 */
#pragma once
#include <stdbool.h>
#include "lexer.h"
#include "vector.h"
#include "ast.h"
#include "Common/include/error.h"

typedef enum {
    PARSER_OK,
    PARSER_ERROR = -1
} parser_status_t;

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

    /** Status of the parser */
    parser_status_t status;

    /** Last error that occured if any */
    error_t * error;
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
 * Returns the status of the parser
 * @returns the status of the parser
 */
parser_status_t Parser_GetStatus(parser_t * parser);

/**
 * Returns the error that the parser encountered if any
 * @retval NULL if no error
 * @retval A pointer to an error if any
 */
error_t * Parser_GetError(parser_t * parser);

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser       The parser used to generate the AST
 * @param     module_scope Whether we parse a module
 * @returns          The AST root node
 */
ast_node_t * Parser_CreateAST(parser_t * parser, bool module_scope);

/**
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
ast_node_t * Parser_ParseObjMsgDef(parser_t * parser);
ast_node_t * Parser_ParseObjLitteral(parser_t * parser);
ast_node_t * Parser_ParseObjFieldName(parser_t * parser);
ast_node_t * Parser_ParseArrayLitteral(parser_t * parser);
ast_node_t * Parser_ParseBlock(parser_t * parser);
ast_node_t * Parser_ParseArrayAccess(parser_t * parser);

/**
 * @retval an ast_node_t<ast_dotted_expr_t> * if there is atleast 2 components (a.b or a[b])
 * @retval an ast_node_t<ast_identifier_t> * if only one component
 */
ast_node_t * Parser_ParseDottedExpr(parser_t * parser);

/**
 * @retval an ast_node_t<ast_msg_pass_expr_t> * if there is atleast 2 components (a b: "hello" or a b)
 * @retval an ast_node_t<T> * if only one component of type T
 */
ast_node_t * Parser_ParseMsgPassExpr(parser_t * parser);
ast_node_t * Parser_ParseExpr(parser_t * parser);

/**
 * @param[in] parser
 * @param     type   NODE_OR_EXPR <= type <= NODE_FACTOR_EXPR
 * @retval an ast_node_t<ast_expr_t> * if there is atleast 2 components (a <op of type> b)
 * @retval an ast_node_t<T> * if only one component of type T
 */
ast_node_t * Parser_ParseBinaryExpr(parser_t * parser, ast_node_type_t type);
ast_node_t * Parser_ParseUnaryExpr(parser_t * parser);
ast_node_t * Parser_ParseStatement(parser_t * parser, bool module_scope);
ast_node_t * Parser_ParseAtomExpr(parser_t * parser);
ast_node_t * Parser_ParseLitteralExpr(parser_t * parser);
