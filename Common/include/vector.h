/**
 * @file vector.h
 * Simple vector implementation
 */
#pragma once
#include <sys/types.h>
#include "nanbox.h"

/**
 * Represents a vector
 * Vectors store nanboxed data
 */
typedef struct {
    /** The buffer where the elements are stored */
    nanbox_t * buffer;

    /** The maximum length of the buffer before any reallocation is needed */
    size_t max_length;

    /** Current length of the vector */
    size_t length;

    /** The length of each length increments */
    size_t increment_length;
} vector_t;

/**
 * Allocates a new vector
 * @returns A pointer to the newly allocated vector
 */
vector_t * Vec_New();

/**
 * Allocates a new vector
 * @param increment_length Length of each increment in reallocation
 * @returns A pointer to the newly allocated vector
 */
vector_t * Vec_NewWithIncrementLength(size_t increment_length);

/**
 * Frees an allocated vector
 * @param[in] vector A pointer to the vector to free
 */
void Vec_Free(vector_t * vector);

/**
 * Retrieves a stored element
 * @param[in] vector A pointer to the vector
 * @param     index  The index of the element in the vector
 * @retval The stored element
 * @retval nanbox_null() in case of an error (or if nanbox_null() was stored...)
 */
nanbox_t Vec_GetAt(vector_t * vector, size_t index);

/**
 * Stores an element
 * @param[in] vector A pointer to the vector
 * @param     index  The index of the element in the vector
 * @param[in] elem   The element to be stored
 */
void Vec_SetAt(vector_t * vector, size_t index, nanbox_t elem);

/**
 * Appends an element to the vector
 * @param[in] vector A pointer to the vector
 * @param[in] elem   The element to be stored
 */
void Vec_Append(vector_t * vector, nanbox_t elem);

/**
 * Pops an element from the vector
 * @param[in] vector A pointer to the vector
 * @returns          The poped element
 */
nanbox_t Vec_Pop(vector_t * vector);

/**
 * Gets the current length of the vector
 * @param[in] vector A pointer to the vector
 * @returns          The length of the vector
 */
size_t Vec_GetLength(vector_t * vector);

/**
 * Gets the current max length of the vector
 * @param[in] vector A pointer to the vector
 * @returns          The max length of the vector
 */
size_t Vec_GetMaxLength(vector_t * vector);

/**
 * Replace each element by the result of the invokation of a
 * given function that is passed the element as argument
 * @param[in] vector A pointer to the vector
 * @param[in] func   The function to use
 */
void Vec_Map(vector_t * vector, nanbox_t (*func)(nanbox_t));

/**
 * Execute a given function on each element
 * @param[in] vector A pointer to the vector
 * @param[in] func   The function to use
 */
void Vec_ForEach(vector_t * vector, void (*func)(nanbox_t));
