/**
 * @file native_block.h
 * Native code block
 */
#pragma once

#include <sys/types.h>
#include "object.h"
#include "objects_types.h"
#include "nanbox.h"

#define NATIVE_BLOCK_SIGNATURE(function_name) void (*function_name)(nanbox_t, nanbox_t)

typedef struct {
    OBJECT_HEAD
    // param_count: integer
    // sourceCode: { text: "[native code]", lineCount: 0 }
    
    /// @todo replace with proper signature
    NATIVE_BLOCK_SIGNATURE(native_function);
} native_block_t;

nanbox_t NativeBlock_New(NATIVE_BLOCK_SIGNATURE(native_function));

/**
 * @todo
 */
nanbox_t NativeBlock_Call(nanbox_t native_block, nanbox_t context);
