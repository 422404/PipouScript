/**
 * @file arrayobject.h
 * Arrays Implementation
 */
#pragma once
#include "object.h"
#include "vector.h"
#include "nanbox.h"

typedef struct {
    OBJECT_HEAD;
    vector_t * items;
} arrayobject_t;

/**
 * Allocates a new arrayobject
 * @returns The newly allocated arrayobject
 */
nanbox_t ArrayObject_New(void);

/**
 * Retrieves a stored element in the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param index       The index of the element in the arrayobject
 * @returns           The stored element
 */
nanbox_t ArrayObject_GetAt(nanbox_t arrayobject, ssize_t index);

/**
 * Stores an element in the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param index      The index of the element in the arrayobject
 * @param item       The element to be stored
 */
void ArrayObject_SetAt(nanbox_t arrayobject, ssize_t index, nanbox_t item);

/**
 * Appends an element to the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param item        The element to be stored
 */
void ArrayObject_Append(nanbox_t arrayobject, nanbox_t item);

/**
 * Pops an element from the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @returns           The poped element
 */
nanbox_t ArrayObject_Pop(nanbox_t arrayobject);

/**
 * Get the length of the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @returns           The arrayobject length
 */
size_t ArrayObject_GetLength(nanbox_t arrayobject);
