/**
 * @file hashmap_tests.c
 * HashMap tests
 */
#include <stdlib.h>
#include "seatest.h"
#include "hashmap.h"
#include "nanbox.h"
#include "vector.h"

static void FreeHashMapValues(nanbox_t value) {
    if (nanbox_is_pointer(value)) {
        free(nanbox_to_pointer(value));
    }
}

static void Test_SetGet(void) {
    hashmap_t * hashmap;
    nanbox_t a, b;

    // we can set (key, val) pairs
    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "abcd", nanbox_from_int(1337));
    HashMap_Set(hashmap, "efgh", nanbox_from_int(1234));
    assert_int_equal(2, hashmap->count);
    HashMap_Get(hashmap, "abcd", &a);
    HashMap_Get(hashmap, "efgh", &b);
    assert_int_equal(1337, nanbox_to_int(a));
    assert_int_equal(1234, nanbox_to_int(b));
    HashMap_Free(hashmap);

    // we can overwrite (key, val) pairs
    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "abcd", nanbox_from_int(1337));
    assert_int_equal(1, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", &a));
    assert_int_equal(1337, nanbox_to_int(a));
    HashMap_Set(hashmap, "abcd", nanbox_from_int(4321));
    assert_int_equal(1, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", &a));
    assert_int_equal(4321, nanbox_to_int(a));
    HashMap_Free(hashmap);

    // collisions don't prevent retreiving values
    hashmap = HashMap_NewWithCapacity(10);
    assert_true(hashmap != NULL);
    // for size == 10, "abcd" and "abcdefg" hash % 10 is the same: 5
    HashMap_Set(hashmap, "abcd", nanbox_from_int(1337));
    HashMap_Set(hashmap, "abcdefg", nanbox_from_int(1234));
    assert_int_equal(2, hashmap->count);
    assert_true(HashMap_Get(hashmap, "abcd", &a));
    assert_true(HashMap_Get(hashmap, "abcdefg", &b));
    assert_int_equal(1337, nanbox_to_int(a));
    assert_int_equal(1234, nanbox_to_int(b));
    HashMap_Free(hashmap);

    // realloc don't cause entries to disappear (1)
    hashmap = HashMap_NewWithCapacity(1);
    assert_true(hashmap != NULL);
    assert_int_equal(1, hashmap->entries_count);
    // the hashmap will realloc more memory because
    // the load factor will be > 0.75 (1.0)
    HashMap_Set(hashmap, "abcd", nanbox_from_int(1337));
    assert_int_equal(2, hashmap->entries_count);
    assert_true(HashMap_Get(hashmap, "abcd", &a));
    assert_int_equal(1337, nanbox_to_int(a));
    HashMap_Free(hashmap);

    // realloc don't cause entries to disappear (2)
    char * collisions[] = {"abcd", "abcdefg", "abcdefghijk", "rp", "foobarbuzz"};
    hashmap = HashMap_NewWithCapacity(10);
    assert_true(hashmap != NULL);
    for (size_t i = 0; i < 5; i++) {
        HashMap_Set(hashmap, collisions[i], nanbox_from_int(i));
    }
    assert_int_equal(5, hashmap->count);
    assert_int_equal(10, hashmap->entries_count);
    // trigger a realloc by putting 3 other elements (8 / 10 > 0.75)
    // (collision between "hello" and "world" :^)
    HashMap_Set(hashmap, "hello", nanbox_from_int(1234));
    HashMap_Set(hashmap, "world", nanbox_from_int(1337));
    HashMap_Set(hashmap, "foo", nanbox_from_int(4321));
    // capacity should be doubled now and count should be the same as before (8)
    assert_int_equal(8, hashmap->count);
    assert_int_equal(20, hashmap->entries_count);
    for (size_t i = 0; i < 5; i++) {
        assert_true(HashMap_Get(hashmap, collisions[i], &a));
        assert_int_equal(i, nanbox_to_int(a));
    }
    assert_true(HashMap_Get(hashmap, "hello", &a));
    assert_int_equal(1234, nanbox_to_int(a));
    assert_true(HashMap_Get(hashmap, "world", &a));
    assert_int_equal(1337, nanbox_to_int(a));
    assert_true(HashMap_Get(hashmap, "foo", &a));
    assert_int_equal(4321, nanbox_to_int(a));
    HashMap_Free(hashmap);
}

void Test_Remove(void) {
    hashmap_t * hashmap;

    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "hello", nanbox_from_int(1337));
    assert_true(HashMap_Contains(hashmap, "hello"));
    assert_int_equal(1, hashmap->count);
    HashMap_Remove(hashmap, "hello");
    assert_false(HashMap_Contains(hashmap, "hello"));
    assert_int_equal(0, hashmap->count);
    HashMap_Free(hashmap);
}

void Test_GetValuesAndKeys(void) {
    hashmap_t * hashmap;
    vector_t * values, * keys;

    hashmap = HashMap_New();
    assert_true(hashmap != NULL);
    HashMap_Set(hashmap, "abcd", nanbox_from_int(1337));
    HashMap_Set(hashmap, "efgh", nanbox_from_int(1234));
    HashMap_Set(hashmap, "ijkl", nanbox_from_double(13.37));
    values = HashMap_GetValues(hashmap);
    assert_int_equal(3, Vec_GetLength(values));
    Vec_Free(values);
    keys = HashMap_GetKeys(hashmap);
    assert_int_equal(3, Vec_GetLength(keys));
    Vec_ForEach(keys, FreeHashMapValues);
    Vec_Free(keys);
    HashMap_Free(hashmap);
}

/**
 * Runs all HashMap tests
 */
void Test_HashMapTests(void) {
    test_fixture_start();
    run_test(Test_SetGet);
    run_test(Test_Remove);
    run_test(Test_GetValuesAndKeys);
    test_fixture_end();
}
