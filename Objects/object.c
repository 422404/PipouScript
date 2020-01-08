/**
 * @file object.c
 * Object system implementation
 */
#include <string.h>
#include <stdlib.h>
#include "object.h"
#include "location.h"
#include "Common/include/error.h"
#include "hashmap.h"
#include "nanbox.h"
#include "vector.h"

nanbox_t Object_New(size_t object_size, OBJECT_CUSTOM_FREE_SIGNATURE(custom_free)) {
    object_t * object = NULL;

    object = (object_t *)malloc(object_size);
    if (object) {
        object->ref_count = 1;
        object->freezed = false;
        object->fields = HashMap_New();
        object->prototype = nanbox_null(); /// @todo base object
        object->type = OBJECT;
        object->custom_free = custom_free;
    } else {
        loc_t loc = { __LINE__, 0, __FILE__ };
        Err_Throw(Err_NewWithLocation("Cannot allocate new object", loc));
    }

    return nanbox_from_pointer(object);
}

void Object_Free(nanbox_t * object) {
    object_t * object_ptr;

    if (nanbox_is_pointer(*object) && (object_ptr = nanbox_to_pointer(*object))) {
        vector_t * fields = HashMap_GetValues(object_ptr->fields);
        nanbox_t proto = Object_GetPrototype(*object);

        for (size_t i = 0; i < Vec_GetLength(fields); i++) {
            nanbox_t field = Vec_GetAt(fields, i);
            if (nanbox_is_pointer(field)) {
                Object_DecRef(&field);
            }
        }
        if (nanbox_is_pointer(proto)) {
            Object_DecRef(&proto);
        }
        Vec_Free(fields);
        HashMap_Free(object_ptr->fields);

        // use custom free function for special objects
        if (object_ptr->custom_free) {
            object_ptr->custom_free(object_ptr);
        }

        free(object_ptr);
        *object = nanbox_deleted();
    } else if (nanbox_is_deleted(*object)) {
        loc_t loc = { __LINE__, 0, __FILE__ };
        Err_Throw(Err_NewWithLocation("Object already freed", loc));
    } else {
        loc_t loc = { __LINE__, 0, __FILE__ };
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

void Object_AddTracker(nanbox_t object, object_tracker_t * object_tracker) {
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        obj_ptr->object_tracker = object_tracker;
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

void Object_IncRef(nanbox_t object) {
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        obj_ptr->ref_count++;
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

void Object_DecRef(nanbox_t * object) {
    if (nanbox_is_pointer(*object)) {
        object_t * obj_ptr = nanbox_to_pointer(*object);
        obj_ptr->ref_count--;
        if (obj_ptr->ref_count < 1) {
            Object_Free(object);
        }
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

void Object_Freeze(nanbox_t object) {
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        obj_ptr->freezed = true;
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

void Object_SetField(nanbox_t object, char * name, nanbox_t value) {
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        HashMap_Set(obj_ptr->fields, name, value);
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

nanbox_t Object_GetField(nanbox_t object, char * name) {
    nanbox_t val = nanbox_null();

    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        if (!HashMap_Get(obj_ptr->fields, name, &val)) {
            nanbox_t proto = Object_GetPrototype(object);
            if (nanbox_is_pointer(proto)) {
                val = Object_GetField(proto, name);
            } else {
                val = nanbox_null();
            }
        }
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }

    return val;
}

void Object_SetPrototype(nanbox_t object, nanbox_t prototype) {
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        obj_ptr->prototype = prototype;
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
}

nanbox_t Object_GetPrototype(nanbox_t object) {
    nanbox_t prototype = nanbox_null();
    if (nanbox_is_pointer(object)) {
        object_t * obj_ptr = nanbox_to_pointer(object);
        prototype = obj_ptr->prototype;
    } else {
        loc_t loc = {__LINE__ + 1, 0, __FILE__};
        Err_Throw(Err_NewWithLocation("NaN boxed value is not an object", loc));
    }
    return prototype;
}
