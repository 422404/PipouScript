/**
 * @file object.h
 * Object system implementation
 */
#pragma once

#include <sys/types.h>
#include <stdbool.h>
#include "hashmap.h"
#include "vector.h"
#include "nanbox.h"
#include "objects_types.h"
#include "object_tracker.h"

typedef struct object_s object_t;

#define OBJECT_CUSTOM_FREE_SIGNATURE(function_name) void (*function_name)(void *)

#define OBJECT_HEAD size_t ref_count; \
    hashmap_t * fields; \
    nanbox_t prototype; \
    bool freezed; \
    object_type_t type; \
    OBJECT_CUSTOM_FREE_SIGNATURE(custom_free); \
    object_tracker_t * object_tracker;

typedef struct object_s {
    OBJECT_HEAD
} object_t;

#define NEW_FROM_TYPE(object_type) Object_New(sizeof(object_type))

nanbox_t Object_New(size_t object_size, OBJECT_CUSTOM_FREE_SIGNATURE(custom_free));
void Object_Free(nanbox_t * object);
void Object_AddTracker(nanbox_t object, object_tracker_t * object_tracker);
void Object_IncRef(nanbox_t object);
void Object_DecRef(nanbox_t * object);
void Object_Freeze(nanbox_t object);
void Object_SetField(nanbox_t object, char * name, nanbox_t value);
nanbox_t Object_GetField(nanbox_t object, char * name);
void Object_SetPrototype(nanbox_t object, nanbox_t prototype);
nanbox_t Object_GetPrototype(nanbox_t object);
