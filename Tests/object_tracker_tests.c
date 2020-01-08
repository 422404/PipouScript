/**
 * @file object_tracker_tests.c
 * Object Tracker Tests
 */
#include "seatest.h"
#include "nanbox.h"
#include "object.h"
#include "arrayobject.h"

void Test_NoLeaks(void) {
    object_tracker_t * tracker = ObjectTracker_New();
    nanbox_t object = Object_New(sizeof(object_t), NULL);
    nanbox_t object2 = Object_New(sizeof(object_t), NULL);
    nanbox_t object3 = Object_New(sizeof(object_t), NULL);
    nanbox_t array = ArrayObject_New();

    Object_SetField(object, "hello", object2);
    Object_SetField(object, "world", nanbox_true());
    ArrayObject_Append(array, nanbox_from_int(1337));
    ArrayObject_Append(array, object3);
    Object_SetField(object2, "another", object3);

    ObjectTracker_Track(tracker, object);
    ObjectTracker_Track(tracker, array);
    // We free all 4 objects
    ObjectTracker_Free(tracker);
}

/**
 * Runs all object tracker tests
 */
void Test_ObjectTrackerTests(void) {
    test_fixture_start();
    run_test(Test_NoLeaks);
    test_fixture_end();
}
