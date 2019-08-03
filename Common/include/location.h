/**
 * @file location.h
 */
#pragma once

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
