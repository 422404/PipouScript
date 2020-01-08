/**
 * @file object_tracker.h
 * Object Tracker Implementation
 */
#pragma once
#include "vector.h"

typedef struct {
    vector_t * objects;
} object_tracker_t;

/**
 * Allocates a new object tracker
 * @return A newly allocated object tracker
 */
object_tracker_t * ObjectTracker_New(void);

/**
 * Track an object.
 * The object will be automatically freed when the
 * object tracker will be freed.
 * @param[in] object_tracker The object tracker
 * @param     object         The object to track
 */
void ObjectTracker_Track(object_tracker_t * object_tracker, nanbox_t object);

/**
 * Frees all the tracked objects then the tracker itself.
 * @param[in] object_tracker The object tracker to be freed
 */
void ObjectTracker_Free(object_tracker_t * object_tracker);
