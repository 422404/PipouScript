/**
 * @file lexer.h
 * Input lexical analysis
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "tokens.h"
#include "token.h"

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
 * @todo Add en error field with a reason and line/col numbers
 * @todo Create an error type
 */
typedef struct {
    /** Input buffer address */
    char * buffer;

    /** Input buffer length */
    size_t buffer_length;

    /**
     * Address of the next char to be processed
     * @todo Remove
     */
    char * current_char;

    /** Current token address in buffer */
    char * token_start;

    /** Hypothetical type of the token to be matched */
    token_type_t token_type;

    /** Current position in the source */
    pos_t pos;
    
    /** Current state of the lexer */
    lexer_status_t status;
} lexer_t;

/**
 * Allocates a new lexer
 * @param[in] buffer The buffer where tokens will be extracted
 * @param     length Length of the input buffer
 * @returns          A pointer to the newly allocated lexer
 */
lexer_t * Lex_New(char * buffer, size_t length);

/**
 * Frees an allocated lexer.
 * Calling the function don't free the input buffer or the
 * extracted tokens
 * @param[in] lexer A pointer to the lexer to be freed
 */
void Lex_Free(lexer_t * lexer);

/**
 * Try to extract the next token in the buffer
 * @param[in]  lexer       The lexer used to extract the token
 * @param      preserve_ws When set to true tokens categorized
 *                         as whitespaces will not be ignored
 * @retval A pointer to the newly extracted token
 * @retval NULL if an error occured
 *         Call Lex_GetStatus() for more info
 */
token_t * Lex_NextToken(lexer_t * lexer, bool preserve_ws);

/**
 * Gets the current status of the lexer
 * @returns The current status of the lexer
 */
lexer_status_t Lex_GetStatus(lexer_t * lexer);
