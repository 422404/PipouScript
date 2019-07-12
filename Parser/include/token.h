/**
 * @file token.h
 * Token implementation
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "tokens.h"

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
