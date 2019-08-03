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

/**
 * Parse the code and create the raw AST for it
 * @param[in] parser The parser used to generate the AST
 * @returns          The AST root node
 */
ast_node_t * Parser_CreateAST(parser_t * parser) {
    UNUSED(parser);
    return NULL;
}

