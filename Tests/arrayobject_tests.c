/**
 * @file arrayobject_tests.c
 * ArrayObject tests
 */
#include <stdio.h>
#include "seatest.h"
#include "nanbox.h"
#include "arrayobject.h"
#include "objects_types.h"

void Test_ArrayObjectCreation(void) {
    nanbox_t arrayobject;
    arrayobject_t * arrayobject_ptr;
    size_t length;

    arrayobject = ArrayObject_New();
    arrayobject_ptr = nanbox_to_pointer(arrayobject);
    assert_int_equal(ARRAY_OBJECT, arrayobject_ptr->type);

    length = nanbox_to_int(Object_GetField(arrayobject, "length"));
    assert_int_equal(0, length);

    Object_DecRef(&arrayobject);
}

void Test_ArrayObjectAppendingPopping(void) {
    nanbox_t arrayobject;
    size_t length;

    arrayobject = ArrayObject_New();
    ArrayObject_Append(arrayobject, nanbox_from_int(1337));
    ArrayObject_Append(arrayobject, nanbox_from_double(13.37));
    length = nanbox_to_int(Object_GetField(arrayobject, "length"));
    assert_int_equal(2, length);
    assert_int_equal(1337, nanbox_to_int(ArrayObject_GetAt(arrayobject, 0)));
    assert_double_equal(13.37, nanbox_to_double(ArrayObject_GetAt(arrayobject, 1)), 0.0);

    ArrayObject_Pop(arrayobject);
    length = nanbox_to_int(Object_GetField(arrayobject, "length"));
    assert_int_equal(1, length);
    assert_int_equal(1, ArrayObject_GetLength(arrayobject));

    Object_DecRef(&arrayobject);
}

void Test_ArrayObjectTests(void) {
    test_fixture_start();
    run_test(Test_ArrayObjectCreation);
    run_test(Test_ArrayObjectAppendingPopping);
    /// @todo add tests
    test_fixture_end();
}
