/**
 * @file iterator_tests.c
 * Iterator tests
 */
#include "seatest.h"
#include "iterator.h"
#include "misc.h"

typedef struct test_iterator_source_s {
    int data[10];
    int pos;
} test_iterator_source_t;


static bool has_next_stub(iterator_t * iterator) {
    UNUSED(iterator);
    return false;
}

static void * next_stub(iterator_t * iterator) {
    UNUSED(iterator);
    return NULL;
}

static bool has_next(iterator_t * iterator) {
    return ((test_iterator_source_t *)iterator->source)->pos < 10;
}

static void * next(iterator_t * iterator) {
    test_iterator_source_t * source = iterator->source;
    void * res = NULL;
    if (Iter_HasNext(iterator)) {
        res = &source->data[source->pos];
        source->pos++;
    }
    return res;
}

/**
 * Tests the creation of an iterator
 */
void Test_IteratorCreation(void) {
    test_iterator_source_t source;
    iterator_t * iterator;

    iterator = Iter_New(&source, has_next_stub, next_stub);
    assert_true(iterator != NULL);
    assert_false(Iter_HasNext(iterator));
    assert_true(Iter_Next(iterator) == NULL);
    Iter_Free(iterator);
}

/**
 * Tests the usage of some iterator implementation
 */
void Test_IteratorIterating(void) {
    test_iterator_source_t source = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, 0
    };
    iterator_t * iterator;

    iterator = Iter_New(&source, has_next, next);
    for (int i = 0; Iter_HasNext(iterator); i++) {
        assert_int_equal(i, *(int *)Iter_Next(iterator));
    }
    Iter_Free(iterator);
}

/**
 * Runs all iterator tests
 */
void Test_IteratorTests(void) {
    test_fixture_start();
    run_test(Test_IteratorCreation);
    run_test(Test_IteratorIterating);
    test_fixture_end();
}
