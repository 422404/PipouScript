/**
 * @file token.c
 * Token implementation
 */
#include <stddef.h>
#include <stdlib.h>
#include "error.h"
#include "token.h"
#include "tokens.h"

/**
 * @param[in] token A pointer to the token
 * @retval true if the token is a whitespace
 * @retval false if it's not
 */
bool Token_IsWhitespace(token_t * token) {
    return token->type == TOKTYPE_SPACE
            || token->type == TOKTYPE_TAB
            || token->type == TOKTYPE_NEWLINE;
}

/**
 * Allocates a new token
 * @param type Token type
 * @param span Span of the token
 * @returns    A pointer to the newly allocated token
 */
token_t * Token_New(token_type_t type, span_t span) {
    token_t * token = NULL;

    token = (token_t *)malloc(sizeof(token_t));
    if (token) {
        token->type = type;
        token->span = span;
    } else {
        Err_Throw(Err_New("Cannot allocated token"));
    }
    return token;
}

/**
 * Frees a token
 * @param[in] token Token to free
 */
void Token_Free(token_t * token) {
    if (token) free(token);
    else Err_Throw(Err_New("NULL pointer to token"));
}
