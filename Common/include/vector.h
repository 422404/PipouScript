/**
 * @file vector.h
 * Simple vector implementation
 */
#pragma once
#include <sys/types.h>

/**
 * Represents a vector
 * Vectors store pointers to arbitrary data
 * In the descriptions "element" will mean "stored pointer"
 */
typedef struct {
    /** The buffer where the elements are stored */
    void ** buffer;

    /**The maximum length of the buffer before any reallocation is needed */
    size_t max_length;

    /** Current length of the vector */
    size_t length;
} vector_t;

/**
 * Allocates a new vector
 * @returns A pointer to the newly allocated vector
 */
vector_t * Vec_New();

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
 * @retval NULL in case of an error (or if NULL was stored...)
 */
void * Vec_GetAt(vector_t * vector, size_t index);

/**
 * Stores an element
 * @param[in] vector A pointer to the vector
 * @param     index  The index of the element in the vector
 * @param[in] elem   The element to be stored
 */
void Vec_SetAt(vector_t * vector, size_t index, void * elem);

/**
 * Appends an element to the vector
 * @param[in] vector A pointer to the vector
 * @param[in] elem   The element to be stored
 */
void Vec_Append(vector_t * vector, void * elem);

/**
 * Pops an element from the vector
 * @param[in] vector A pointer to the vector
 * @returns          The poped element
 */
void * Vec_Pop(vector_t * vector);

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
