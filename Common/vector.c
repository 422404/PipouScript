/**
 * @file vector.c
 * Simple vector implementation
 */
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include "error.h"
#include "vector.h"

#define LENGTH_INCREMENT 50

/**
 * Tests if an index is in the bounds of a vector
 * @private
 * @param[in] vector A pointer to the vector
 * @param     index  The index to test
 * @retval           true if in the bounds
 * @retval           false if not
 */
static bool Vec_InBounds(vector_t * vector, size_t index) {
    return 0 <= index && index < vector->length;
}

/**
 * Checks if a vector can be appended an element or if it's at
 * its maximum capacity
 * @private
 * @param[in] vector A pointer to the vector
 * @retval true if the vector must grow to store another element
 * @retval false if the vector is not full
 */
static bool Vec_MustGrow(vector_t * vector) {
    return 1 + vector->length > vector->max_length;
}

/**
 * Grows the vector by LENGTH_INCREMENT elements
 * @private
 * @param[in] vector A pointer to the vector
 */
static void Vec_Grow(vector_t * vector) {
    void * buf;
    size_t new_length = vector->max_length + LENGTH_INCREMENT;
    buf = realloc(vector->buffer, new_length * sizeof(void *));
    if (buf) {
        vector->buffer = buf;
        vector->max_length = new_length;
    }
}

/**
 * Allocates a new vector
 * @todo Ability to adjust LENGTH_INCREMENT
 * @returns A pointer to the newly allocated vector
 */
vector_t * Vec_New() {
    vector_t * vector = (vector_t *)malloc(sizeof(vector_t));

    if (vector) {
        vector->length = vector->max_length = 0;
        vector->buffer = NULL;
    } else {
        Err_Throw(Err_New("Cannot allocate vector"));
    }
    return vector;
}

/**
 * Frees an allocated vector
 * @param[in] vector A pointer to the vector to free
 */
void Vec_Free(vector_t * vector) {
    if (vector) {
        if (vector->buffer) free(vector->buffer);
        free(vector);
    }
}

/**
 * Retrieves a stored element
 * @param[in] vector A pointer to the vector
 * @param     index  The index of the element in the vector
 * @retval The stored element
 * @retval NULL in case of an error (or if NULL was stored...)
 */
void * Vec_GetAt(vector_t * vector, size_t index) {
    void * elem = NULL;

    if (vector && Vec_InBounds(vector, index)) {
        elem = vector->buffer[index];
    }
    return elem;
}

/**
 * Stores an element
 * @param[in] vector A pointer to the vector
 * @param     index  The index of the element in the vector
 * @param[in] elem   The element to be stored
 */
void Vec_SetAt(vector_t * vector, size_t index, void * elem) {
    if (vector && Vec_InBounds(vector, index)) {
        vector->buffer[index] = elem;
    }
}

/**
 * Appends an element to the vector
 * @param[in] vector A pointer to the vector
 * @param[in] elem   The element to be stored
 */
void Vec_Append(vector_t * vector, void * elem) {
    if (Vec_MustGrow(vector)) Vec_Grow(vector);
    vector->buffer[vector->length] = elem;
    vector->length++;
}

/**
 * Pops an element from the vector
 * @param[in] vector A pointer to the vector
 * @returns          The poped element
 * 
 * @todo Think about reallocating while poping
 */
void * Vec_Pop(vector_t * vector) {
    void * elem = NULL;

    if (vector->length > 0) {
        vector->length--;
        elem = vector->buffer[vector->length];
    }
    return elem;
}

/**
 * Gets the current length of the vector
 * @param[in] vector A pointer to the vector
 * @returns          The length of the vector
 */
size_t Vec_GetLength(vector_t * vector) {
    return vector->length;
}

/**
 * Gets the current max length of the vector
 * @param[in] vector A pointer to the vector
 * @returns          The max length of the vector
 */
size_t Vec_GetMaxLength(vector_t * vector) {
    return vector->max_length;
}
