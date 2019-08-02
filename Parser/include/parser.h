/**
 * @file parser.h
 * Parser implementation
 */
#pragma once
#include <sys/types.h>
#include "vector.h"
#include "misc.h"
#include "ast.h"

/**
 * Represents a grammar's matched rule
 * @todo remove
 */
typedef struct {
    /** Inner tokens of the matched rule */
    vector_t * tokens;
    
    /** Type of the matched rule */
    ast_node_type_t type;
} rule_match_t;

/**
 * Represents a location in code
 */
typedef struct {
    size_t line;
    size_t col;
    char * filename;
} loc_t;

/**
 * Represents an arbitrary region that span over
 * two positions
 */
typedef struct {
    /** Start location */
    loc_t start;

    /** End location */
    loc_t end;
} span_t;

typedef struct {
    /**
     * When true the file is parser for "statement" grammar's rules
     * When false the file is parser for "mod_statement" grammar's rules
     */
    bool module_mode;

    /** File that contains the code */
    char * filename;
} parser_t;
