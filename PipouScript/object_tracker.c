/**
 * @file object_tracker.c
 * Object Tracker Implementation
 */
#include <stdlib.h>
#include "object.h"
#include "object_tracker.h"
#include "vector.h"
#include "nanbox.h"
#include "Common/include/error.h"

static void ObjectTracker_FreeObject(nanbox_t object) {
    if (nanbox_is_pointer(object)) {
        Object_Free(&object);
    }
}

/**
 * Allocates a new object tracker
 * @return A newly allocated object tracker
 */
object_tracker_t * ObjectTracker_New(void)  {
    object_tracker_t * object_tracker;

    object_tracker = (object_tracker_t *)malloc(sizeof(object_tracker_t));
    if (object_tracker) {
        object_tracker->objects = Vec_New();
    } else {
        Err_Throw(Err_New("Cannot allocate object tracker"));
    }

    return object_tracker;
}

/**
 * Track an object.
 * The object will be automatically freed when the
 * object tracker will be freed.
 * @param[in] object_tracker The object tracker
 * @param     object         The object to track
 */
void ObjectTracker_Track(object_tracker_t * object_tracker, nanbox_t object) {
    Vec_Append(object_tracker->objects, object);
    Object_AddTracker(object, object_tracker);
}

/**
 * Frees all the tracked objects then the tracker itself.
 * @param[in] object_tracker The object tracker to be freed
 */
void ObjectTracker_Free(object_tracker_t * object_tracker) {
    if (object_tracker) {
        Vec_ForEach(object_tracker->objects, ObjectTracker_FreeObject);
        Vec_Free(object_tracker->objects);
        free(object_tracker);
    }
}
