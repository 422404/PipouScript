#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vector.h"

#define LENGTH_INCREMENT 50

static bool InVectorBounds(vector_t * vector, size_t index) {
    return 0 <= index && index < vector->length;
}

static bool MustGrowVector(vector_t * vector) {
    return 1 + vector->length > vector->max_length;
}

static void GrowVector(vector_t * vector) {
    void * buf;
    size_t new_length = vector->max_length + LENGTH_INCREMENT;
    buf = realloc(vector->buffer, new_length);
    if (buf) {
        vector->buffer = buf;
        vector->max_length = new_length;
    }
}

vector_t * Vec_New() {
    vector_t * vector = (vector_t *)malloc(sizeof(vector_t));
    vector->length = vector->max_length = 0;
    vector->buffer = NULL;
    return vector;
}

void Vec_Free(vector_t * vector) {
    if (vector) free(vector);
}

void * Vec_GetAt(vector_t * vector, size_t index) {
    void * elem = NULL;

    if (vector && InVectorBounds(vector, index)) {
        elem = vector->buffer[index];
    }
    return elem;
}

void Vec_SetAt(vector_t * vector, size_t index, void * elem) {
    if (vector && InVectorBounds(vector, index)) {
        vector->buffer[index] = elem;
    }
}

void Vec_Append(vector_t * vector, void * elem) {
    if (MustGrowVector(vector)) GrowVector(vector);
    vector->buffer[vector->length] = elem;
    vector->length++;
}

void * Vec_Pop(vector_t * vector) {
    void * elem = NULL;

    if (vector->length > 0) {
        vector->length--;
        elem = vector->buffer[vector->length];
    }
    return elem;
}

size_t Vec_GetLength(vector_t * vector) {
    return vector->length;
}

size_t Vec_GetMaxLength(vector_t * vector) {
    return vector->max_length;
}
