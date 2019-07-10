/**
 * @file lexer.h
 * Input lexical analysis
 */
#pragma once
#include <sys/types.h>
#include "misc.h"

/**
 * Represents a position in the source code
 */
typedef struct {
    /**
     * The column of the position
     * Starts at 1
     */
    size_t col;

    /** 
     * The line of the position
     * Starts at 1
     */
    size_t line;
} pos_t;

/**
 * Represents an arbitrary region that span over
 * two positions
 */
typedef struct {
    /** Start position */
    pos_t start;

    /** End position */
    pos_t end;
} span_t;

/**
 * Represents a grammar base token
 */
typedef struct {
    /** Token type */
    int type;

    /** Token span */
    span_t span;
} token_t;

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
    size_t* buffer_length;

    /** Address of the next char to be processed */
    char * current_char;

    /** Current token address in buffer */
    char * token_start;

    /** Hypothetical type of the token to be matched */
    int token_type;

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
lexer_t *  Lex_New(char * buffer, size_t length);

/**
 * Frees an allocated lexer.
 * Calling the function don't free the input buffer or the
 * extracted tokens
 * @param[in] lexer A pointer to the lexer to be freed
 */
void Lex_Free(lexer_t * lexer);

/**
 * Try to extract the next token in the buffer
 * @param[in]  lexer The lexer used to extract the token
 * @param[out] token A pointer where will be passed the adderess
 *                   of the newly extracted token
 * @retval 0 if no error occured
 * @retval Non-zero if an error occured.
 *         Call Lex_GetStatus() for more info
 */
int Lex_NextToken(lexer_t * lexer, token_t ** token);

/**
 * Gets the current status of the lexer
 * @returns The current status of the lexer
 */
lexer_status_t Lex_GetStatus(lexer_t * lexer);
