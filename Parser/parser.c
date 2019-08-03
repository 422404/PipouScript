/**
 * @file parser.c
 * Parser implementation
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "Parser/include/parser.h"
#include "error.h"
#include "lexer.h"
#include "vector.h"
#include "misc.h"
#include "ast.h"

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
        if (parser->lexer) free(parser->lexer);
        free(parser);
    }
    else Err_Throw(Err_New("NULL pointer to parser"));
}

static ast_node_t * Parser_ParseIdentifier(parser_t * parser);
static ast_node_t * Parser_ParseString(parser_t * parser);
static ast_node_t * Parser_ParseInt(parser_t * parser);
static ast_node_t * Parser_ParseDouble(parser_t * parser);
static ast_node_t * Parser_ParseDecl(parser_t * parser);
static ast_node_t * Parser_ParseAffect(parser_t * parser);
static ast_node_t * Parser_ParseObjFieldInit(parser_t * parser);
static ast_node_t * Parser_ParseMsgSel(parser_t * parser);
static ast_node_t * Parser_ParseObjMsgDef(parser_t * parser);
static ast_node_t * Parser_ParseObjLitteral(parser_t * parser);
static ast_node_t * Parser_ParseObjFieldName(parser_t * parser);
static ast_node_t * Parser_ParseArrayLitteral(parser_t * parser);
static ast_node_t * Parser_ParseBlock(parser_t * parser);
static ast_node_t * Parser_ParseArrayAccess(parser_t * parser);
static ast_node_t * Parser_ParseDottedExpr(parser_t * parser);
static ast_node_t * Parser_ParseMsgPassExpr(parser_t * parser);
static ast_node_t * Parser_ParseExpr(parser_t * parser);
static ast_node_t * Parser_ParseStatement(parser_t * parser);

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser The parser used to generate the AST
 * @returns          The AST root node
 */
ast_node_t * Parser_CreateAST(parser_t * parser) {
    UNUSED(parser);
    return ASTNode_New(NODE__ROOT_);
}
