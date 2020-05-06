/**
 * @file objects_types.h
 */
#pragma once

typedef enum {
    OBJECT,
    STRING,
    INTEGER,  ///< Unused
    DOUBLE,   ///< Unused
    ARRAY_OBJECT,
    BLOCK,
    NATIVE_BLOCK
} object_type_t;
