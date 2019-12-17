/**
 * @file vector_tests.c
 * Vector tests
 */
#include <assert.h>
#include <stdio.h>
#include "seatest.h"
#include "vector.h"

/**
 * Tests if the vector is created as intended
 */
void Test_VectorCreation(void) {
    vector_t * vector;

    vector = Vec_New();
    assert_true(vector != NULL);
    assert_int_equal(0, Vec_GetLength(vector));
    assert_int_equal(0, Vec_GetMaxLength(vector));
    Vec_Free(vector);
}

/**
 * Tests basic appending to the vector
 */
void Test_VectorBasicAppending(void) {
    vector_t * vector;
    int a = 1337, b = 1234;
    size_t max_length;

    vector = Vec_New();
    Vec_Append(vector, nanbox_from_int(a));
    assert_int_equal(1, Vec_GetLength(vector));
    max_length = Vec_GetMaxLength(vector);
    assert_true(max_length > 0);
    assert_int_equal(1337, nanbox_to_int(Vec_GetAt(vector, 0)));

    Vec_Append(vector, nanbox_from_int(b));
    assert_int_equal(2, Vec_GetLength(vector));
    // no further allocation
    assert_true(Vec_GetMaxLength(vector) == max_length);
    assert_int_equal(1234, nanbox_to_int(Vec_GetAt(vector, 1)));
    Vec_Free(vector);
}

/**
 * Tests appending that need reallocation
 */
void Tests_VectorAdvancedAppending(void) {
    vector_t * vector;
    int a = 1337;
    size_t initial_max_length;

    vector = Vec_New();
    // append one element allocating memory and initializing max_length
    Vec_Append(vector, nanbox_from_int(a));
    initial_max_length = Vec_GetMaxLength(vector);
    // append initial_max_length elements so that it triggers a reallocation
    // (length is currently 1)
    for (size_t i = 0; i < initial_max_length; i++) {
        Vec_Append(vector, nanbox_from_int(a));
    }
    // vector max_length must have doubled now
    assert_int_equal(initial_max_length * 2, Vec_GetMaxLength(vector));
    assert_int_equal(initial_max_length + 1, Vec_GetLength(vector));
    Vec_Free(vector);
}

/**
 * Tests vector access and bound checking
 */
void Test_VectorAccess(void) {
    vector_t * vector;
    nanbox_t elem;
    int a = 1337, b = 1234;

    vector = Vec_New();
    elem = Vec_GetAt(vector, 10);
    assert_true(nanbox_is_null(elem));
    elem = Vec_GetAt(vector, 0);
    assert_true(nanbox_is_null(elem));

    Vec_Append(vector, nanbox_from_int(a));
    Vec_Append(vector, nanbox_from_int(b));
    assert_int_equal(2, Vec_GetLength(vector));
    assert_int_equal(1337, nanbox_to_int(Vec_GetAt(vector, 0)));
    assert_int_equal(1234, nanbox_to_int(Vec_GetAt(vector, 1)));

    elem = Vec_Pop(vector);
    assert_true(nanbox_to_int(elem) == b);
    assert_int_equal(1, Vec_GetLength(vector));
    Vec_SetAt(vector, 0, nanbox_from_int(b));
    assert_int_equal(1234, nanbox_to_int(Vec_GetAt(vector, 0)));

    Vec_Pop(vector);
    assert_int_equal(0, Vec_GetLength(vector));
    // we don't get a negative length
    Vec_Pop(vector);
    assert_int_equal(0, Vec_GetLength(vector));
    Vec_Free(vector);
}

/**
 * Runs all the vector tests
 */
void Test_VectorTests(void) {
    test_fixture_start();
    run_test(Test_VectorCreation);
    run_test(Test_VectorBasicAppending);
    run_test(Tests_VectorAdvancedAppending);
    run_test(Test_VectorAccess);
    test_fixture_end();
}
