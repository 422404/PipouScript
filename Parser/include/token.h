/**
 * @file token.h
 * Token implementation
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "tokens.h"
#include "location.h"

/**
 * Represents a grammar base token
 */
typedef struct {
    /** Token type */
    token_type_t type;

    /** Token span */
    span_t span;
} token_t;

/**
 * @param[in] token A pointer to the token
 * @retval true if the token is a whitespace
 * @retval false if it's not
 */
bool Token_IsWhitespace(token_t * token);

/**
 * Allocates a new token
 * @param type Token type
 * @param span Span of the token
 * @returns    A pointer to the newly allocated token
 */
token_t * Token_New(token_type_t type, span_t span);

/**
 * Frees a token
 * @param[in] token Token to free
 */
void Token_Free(token_t * token);

/**
 * Builds a string representation of the token
 * @param[in] token Token to build the string from
 * @returns         The string representaton of the token
 */
char * Token_ToString(token_t * token);
