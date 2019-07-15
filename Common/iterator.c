/**
 * @file iterator.c
 * Simple iterator implementation
 */
#include <stdlib.h>
#include "error.h"
#include "misc.h"
#include "iterator.h"

/**
 * Allocates a new iterator
 * @param[in] HasNext_impl Pointer to the implementation of the HasNext() function
 * @param[in] Next_impl    Pointer to the implementation of the Next() function
 * @returns A pointer to the newly allocated iterator
 */
iterator_t * Iter_New(void * source, iterator_has_next_func_t HasNext_impl,
                      iterator_next_func_t Next_impl) {
    iterator_t * iterator;

    iterator = (iterator_t *)malloc(sizeof(iterator_t));
    if (iterator) {
        iterator->source  = source;
        iterator->HasNext = HasNext_impl;
        iterator->Next    = Next_impl;
    } else {
        Err_Throw(Err_New("Cannot allocate iterator"));
    }
    return iterator;
}

/**
 * Frees an allocated iterator
 * @param[in] iterator A pointer to the iterator
 */
void Iter_Free(iterator_t * iterator) {
    if (iterator) free(iterator);
    else Err_Throw(Err_New("NULL pointer to iterator"));
}

/**
 * Shothand for calling iterator->HasNext(iterator)
 * @param[in] iterator Iterator pointer
 * @retval true if another element can be iterated on
 * @retval false if no more iterable data is present
 */
bool Iter_HasNext(iterator_t * iterator) {
    bool res = false;

    if (iterator) {
        res = iterator->HasNext(iterator);
    } else {
        Err_Throw(Err_New("NULL pointer to iterator"));
    }
    return res;
}

/**
 * Shothand for calling iterator->Next(iterator)
 * @param[in] iterator Iterator pointer
 * @returns            Next element in the source
 */
void * Iter_Next(iterator_t * iterator) {
    void * res = NULL;

    if (iterator) {
        res = iterator->Next(iterator);
    } else {
        Err_Throw(Err_New("NULL pointer to iterator"));
    }
    return res;
}
