/**
 * @file token.c
 * Token implementation
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
 * @param     type  Token type
 * @param     span  Span of the token
 * @param[in] value Value of the token in case of a "special" token
 *                  The string is duplicated so the caller can free its copy
 * @returns         A pointer to the newly allocated token
 */
token_t * Token_New(token_type_t type, span_t span, char * value) {
    token_t * token = NULL;

    token = (token_t *)malloc(sizeof(token_t));
    if (token) {
        token->type = type;
        token->span = span;
        token->value = value ? strdup(value) : NULL;
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
    if (token) {
        if (token->value) free(token->value);
        free(token);
    } else {
        Err_Throw(Err_New("NULL pointer to token"));
    }
}

/**
 * Builds a string representation of the token
 * @param[in] token Token to build the string from
 * @returns         The string representaton of the token
 */
char * Token_ToString(token_t * token) {
    const size_t string_buf_len = 100;
    char * string_buf = (char *)malloc(string_buf_len);
    if (string_buf) {
        snprintf(string_buf, string_buf_len, "Token { %s, (%ld:%ld), (%ld:%ld), %s }",
            token_type_names[token->type],
            token->span.start.line, token->span.start.col,
            token->span.end.line,   token->span.end.col,
            token->value ? token->value : "<null>");
    }
    return string_buf;
}
