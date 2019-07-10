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
    assert_int_equal(0, vector->length);
    assert_int_equal(0, vector->max_length);
}

/**
 * Runs all the vector tests
 */
void Test_VectorTests(void) {
    test_fixture_start();
    run_test(Test_VectorCreation);
    test_fixture_end();
}
