/**
 * @file native_block.c
 * Native code block
 */
#include <sys/types.h>
#include "native_block.h"
#include "object.h"
#include "nanbox.h"

nanbox_t NativeBlock_New(NATIVE_BLOCK_SIGNATURE(native_function)) {
    nanbox_t block = Object_New(sizeof(native_block_t), NULL);
    native_block_t * block_ptr = nanbox_to_pointer(block);
    block_ptr->type = NATIVE_BLOCK;
    block_ptr->native_function = native_function;
    return block;
}
