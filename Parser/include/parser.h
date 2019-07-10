/**
 * @file parser.h
 * Input parsing
 */
#pragma once
#include "lexer.h"
#include "vector.h"
#include "misc.h"

/**
 * Represents a grammar's matched rule
 */
typedef struct {
    /** Inner tokens of the matched rule */
    vector_t * tokens;
    
    /** Type of the matched rule */
    int type;
} rule_match_t;
