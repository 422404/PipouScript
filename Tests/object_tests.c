/**
 * @file object_tests.h
 * Object system tests
 */
#include "seatest.h"
#include "nanbox.h"
#include "object.h"

void Test_SimpleFieldAccess(void) {
    nanbox_t object, val;

    object = Object_New(sizeof(object_t), NULL);
    assert_true(nanbox_is_pointer(object));
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_null(val));

    Object_SetField(object, "hello", nanbox_from_int(1337));
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_int(val));
    assert_int_equal(1337, nanbox_to_int(val));

    Object_DecRef(&object);
    assert_true(nanbox_is_deleted(object));
}

void Test_FieldAccessViaPrototype(void) {
    nanbox_t object, proto, val;

    object = Object_New(sizeof(object_t), NULL);
    assert_true(nanbox_is_pointer(object));
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_null(val));

    proto = Object_New(sizeof(object_t), NULL);
    assert_true(nanbox_is_pointer(proto));
    Object_SetField(proto, "hello", nanbox_from_int(1337));
    Object_SetPrototype(object, proto);
    
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_int(val));
    assert_int_equal(1337, nanbox_to_int(val));

    Object_DecRef(&object);
    assert_true(nanbox_is_deleted(object));
    // assert_true(nanbox_is_deleted(proto));
}

void Test_PrototypeFieldValueOverride(void) {
    nanbox_t object, proto, val;

    object = Object_New(sizeof(object_t), NULL);
    assert_true(nanbox_is_pointer(object));
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_null(val));

    proto = Object_New(sizeof(object_t), NULL);
    assert_true(nanbox_is_pointer(proto));
    Object_SetField(proto, "hello", nanbox_from_int(1337));
    Object_SetPrototype(object, proto);
    
    // object should get the value from its prototype
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_int(val));
    assert_int_equal(1337, nanbox_to_int(val));

    // override the value at "hello" in object
    Object_SetField(object, "hello", nanbox_from_int(1234));
    // the value is into object and overrides the one in its prototype
    val = Object_GetField(object, "hello");
    assert_true(nanbox_is_int(val));
    assert_int_equal(1234, nanbox_to_int(val));

    // the value in the prototype has not changed
    val = Object_GetField(proto, "hello");
    assert_true(nanbox_is_int(val));
    assert_int_equal(1337, nanbox_to_int(val));

    Object_DecRef(&object);
    assert_true(nanbox_is_deleted(object));
    // assert_true(nanbox_is_deleted(proto));
}

/**
 * Runs all object system tests
 */
void Test_ObjectTests(void) {
    test_fixture_start();
    run_test(Test_SimpleFieldAccess);
    run_test(Test_FieldAccessViaPrototype);
    run_test(Test_PrototypeFieldValueOverride);
    test_fixture_end();
}
