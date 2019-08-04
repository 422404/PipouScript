/**
 * @file lexer.h
 * Input lexical analysis
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "error.h"
#include "token.h"
#include "location.h"

/**
 * Represents the current status of the lexer
 */
typedef enum {
    LEX_OK,        ///< The buffer is not fully processed
    LEX_EOF,       ///< The buffer is fully processed, no more token extraction possible
    LEX_ERROR = -1 ///< An error occured while attempting to extract a token
} lexer_status_t;

/**
 * Represents the lexer used to extract tokens from an input buffer
 */
typedef struct {
    /** Input buffer address */
    char * buffer;

    /** Input buffer length */
    size_t buffer_length;

    /** Address of the next char to be processed */
    char * current_char;

    /** Current position in the source */
    loc_t pos;

    /** File that contains the code */
    char * filename;
    
    /** Current status of the lexer */
    lexer_status_t status;
} lexer_t;

/**
 * Allocates a new lexer
 * @param[in] buffer   The buffer where tokens will be extracted
 * @param     length   Length of the input buffer
 * @param[in] filename Name of the file that contains the code (NULL if in REPL mode)
 * @returns            A pointer to the newly allocated lexer
 */
lexer_t * Lex_New(char * buffer, size_t length, char * filename);

/**
 * Frees an allocated lexer.
 * Calling the function don't free the input buffer or the
 * extracted tokens
 * @param[in] lexer A pointer to the lexer to be freed
 */
void Lex_Free(lexer_t * lexer);

/**
 * Try to extract the next token in the buffer
 * @param[in]  lexer                The lexer used to extract the token
 * @param      preserve_whitespaces When set to true tokens categorized
 *                                  as whitespaces will not be ignored
 * @param      preserve_comments    When set to true comments will not be ignored
 * @retval A pointer to the newly extracted token
 * @retval NULL if an error occured
 *         Call Lex_GetStatus() for more info
 */
token_t * Lex_NextToken(lexer_t * lexer, bool preserve_whitespaces, bool preserve_comments);

/**
 * Gets the current status of the lexer
 * @returns The current status of the lexer
 */
lexer_status_t Lex_GetStatus(lexer_t * lexer);
