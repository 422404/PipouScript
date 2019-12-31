/**
 * @file arrayobject.c
 * Arrays Implementation
 */
#include "arrayobject.h"
#include "object.h"
#include "objects_types.h"
#include "nanbox.h"
#include "vector.h"

#define LENGTH_FIELD "length"

static void ArrayObject_FreeItem(nanbox_t item) {
    if (nanbox_is_pointer(item)) {
        Object_DecRef(&item);
    }
}

static void ArrayObject_CustomFree(void * object_ptr) {
    arrayobject_t * arrayobject_ptr = (arrayobject_t *)object_ptr;
    Vec_ForEach(arrayobject_ptr->items, ArrayObject_FreeItem);
    Vec_Free(arrayobject_ptr->items);
}

/**
 * Allocates a new arrayobject
 * @returns The newly allocated arrayobject
 */
nanbox_t ArrayObject_New(void) {
    arrayobject_t * arrayobject_ptr;
    nanbox_t arrayobject;
    arrayobject = Object_New(sizeof(arrayobject_t), ArrayObject_CustomFree);
    arrayobject_ptr = nanbox_to_pointer(arrayobject);
    arrayobject_ptr->type = ARRAY_OBJECT;
    arrayobject_ptr->items = Vec_New();
    /// @todo Array prototype
    // Object_SetPrototype(arrayobject, /* TODO */);
    // Initial length is 0
    Object_SetField(arrayobject, LENGTH_FIELD, nanbox_from_int(0));
    return arrayobject;
}

/**
 * Retrieves a stored element in the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param index       The index of the element in the arrayobject
 * @returns           The stored element
 */
nanbox_t ArrayObject_GetAt(nanbox_t arrayobject, ssize_t index) {
    arrayobject_t * arrayobject_ptr = nanbox_to_pointer(arrayobject);
    size_t vec_size = Vec_GetLength(arrayobject_ptr->items);
    if (index < 0 || index > vec_size - 1) {
        /// @todo Raise exception
        return nanbox_null();
    }
    return Vec_GetAt(arrayobject_ptr->items, index);
}

/**
 * Stores an element in the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param index      The index of the element in the arrayobject
 * @param item       The element to be stored
 */
void ArrayObject_SetAt(nanbox_t arrayobject, ssize_t index, nanbox_t item) {
    arrayobject_t * arrayobject_ptr = nanbox_to_pointer(arrayobject);
    size_t vec_size = Vec_GetLength(arrayobject_ptr->items);
    if (index < 0 || index > vec_size - 1) {
        /// @todo Raise exception
    } else {
        Vec_SetAt(arrayobject_ptr->items, index, item);
    }
}

/**
 * Appends an element to the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @param item        The element to be stored
 */
void ArrayObject_Append(nanbox_t arrayobject, nanbox_t item) {
    arrayobject_t * arrayobject_ptr = nanbox_to_pointer(arrayobject);
    size_t length = nanbox_to_int(Object_GetField(arrayobject, LENGTH_FIELD));
    Object_SetField(arrayobject, LENGTH_FIELD, nanbox_from_int(length + 1));
    Vec_Append(arrayobject_ptr->items, item);
}

/**
 * Pops an element from the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @returns           The poped element
 */
nanbox_t ArrayObject_Pop(nanbox_t arrayobject) {
    arrayobject_t * arrayobject_ptr = nanbox_to_pointer(arrayobject);
    size_t length = nanbox_to_int(Object_GetField(arrayobject, LENGTH_FIELD));
    if (length > 1) {
        Object_SetField(arrayobject, LENGTH_FIELD, nanbox_from_int(length - 1));
        return Vec_Pop(arrayobject_ptr->items);
    }
    /// @todo Raise exception
    return nanbox_null();
}

/**
 * Get the length of the arrayobject
 * @param arrayobject A reference to the arrayobject
 * @returns           The arrayobject length
 */
size_t ArrayObject_GetLength(nanbox_t arrayobject) {
    arrayobject_t * arrayobject_ptr = nanbox_to_pointer(arrayobject);
    return Vec_GetLength(arrayobject_ptr->items);
}
