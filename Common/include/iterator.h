/**
 * @file iterator.h
 * Simple iterator implementation
 */
#pragma once
#include <stdbool.h>

struct iterator_s;

/**
 * Signature of the HasNext() function
 * @param[in] iterator Iterator pointer
 * @retval true if another element can be iterated on
 * @retval false if no more iterable data is present
 */
typedef bool (*iterator_has_next_func_t)(struct iterator_s *);

/**
 * Signature of the Next() function
 * @param[in] iterator Iterator pointer
 * @returns            Next element in the source
 */
typedef void * (*iterator_next_func_t)(struct iterator_s *);

typedef struct iterator_s {
    /** Source of the iterable data */
    void * source;

    /** Pointer to the implementation of the HasNext() function */
    iterator_has_next_func_t HasNext;

    /** Pointer to the implementation of the Next() function */
    iterator_next_func_t Next;
} iterator_t;

/**
 * Allocates a new iterator
 * @param[in] HasNext_impl Pointer to the implementation of the HasNext() function
 * @param[in] Next_impl    Pointer to the implementation of the Next() function
 * @returns A pointer to the newly allocated iterator
 */
iterator_t * Iter_New(void * source, iterator_has_next_func_t HasNext_impl,
                      iterator_next_func_t Next_impl);

/**
 * Frees an allocated iterator
 * @param[in] iterator A pointer to the iterator
 */
void Iter_Free(iterator_t * iterator);

/**
 * Shothand for calling iterator->HasNext(iterator)
 * @param[in] iterator Iterator pointer
 * @retval true if another element can be iterated on
 * @retval false if no more iterable data is present
 */
bool Iter_HasNext(iterator_t * iterator);

/**
 * Shothand for calling iterator->Next(iterator)
 * @param[in] iterator Iterator pointer
 * @returns            Next element in the source
 */
void * Iter_Next(iterator_t * iterator);
