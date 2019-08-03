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
