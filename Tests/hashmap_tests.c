/**
 * @file hashmap_tests.c
 * HashMap tests
 */
#include "seatest.h"
#include "hashmap.h"

static void Test_SetGet(void) {
    hashmap_t * hashmap;
    size_t a, b;

    // we can set (key, val) pairs
    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "abcd", (void *)1337);
    HashMap_Set(hashmap, "efgh", (void *)1234);
    assert_int_equal(2, hashmap->count);
    HashMap_Get(hashmap, "abcd", (void **)&a);
    HashMap_Get(hashmap, "efgh", (void **)&b);
    assert_int_equal(1337, a);
    assert_int_equal(1234, b);
    HashMap_Free(hashmap);

    // we can overwrite (key, val) pairs
    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "abcd", (void *)1337);
    assert_int_equal(1, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", (void **)&a));
    assert_int_equal(1337, a);
    HashMap_Set(hashmap, "abcd", (void *)4321);
    assert_int_equal(1, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", (void **)&a));
    assert_int_equal(4321, a);
    HashMap_Free(hashmap);

    // collisions don't prevent retreiving values
    hashmap = HashMap_NewWithCapacity(10);
    assert_true(hashmap != NULL);
    // for size == 10, "abcd" and "abcdefg" hash % 10 is the same: 5
    HashMap_Set(hashmap, "abcd", (void *)1337);
    HashMap_Set(hashmap, "abcdefg", (void *)1234);
    assert_int_equal(2, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", (void **)&a));
    assert_true(HashMap_Get(hashmap, "abcdefg", (void **)&b));
    assert_int_equal(1337, a);
    assert_int_equal(1234, b);
    HashMap_Free(hashmap);
}

void Test_Remove(void) {
    hashmap_t * hashmap;

    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "hello", (void *)1337);
    assert_true(HashMap_Contains(hashmap, "hello"));
    assert_int_equal(1, hashmap->count);
    HashMap_Remove(hashmap, "hello");
    assert_false(HashMap_Contains(hashmap, "hello"));
    assert_int_equal(0, hashmap->count);
    HashMap_Free(hashmap);
}

/**
 * Runs all HashMap tests
 */
void Test_HashMapTests(void) {
    test_fixture_start();
    run_test(Test_SetGet);
    run_test(Test_Remove);
    test_fixture_end();
}
