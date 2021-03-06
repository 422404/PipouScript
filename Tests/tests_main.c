/**
 * @file tests_main.c
 * Test runner entrypoint
 */
#include "seatest.h"
#include "vector_tests.h"
#include "iterator_tests.h"
#include "lexer_tests.h"
#include "parser_tests.h"
#include "str_tests.h"
#include "hashmap_tests.h"
#include "object_tests.h"
#include "arrayobject_tests.h"
#include "object_tracker_tests.h"

/**
 * Runs all tests
 */
void Test_AllTests(void) {
    Test_VectorTests();
    Test_IteratorTests();
    Test_LexerTests();
    Test_ParserTests();
    Test_StringTests();
    Test_HashMapTests();
    Test_ObjectTests();
    Test_ArrayObjectTests();
    Test_ObjectTrackerTests();
}

/**
 * Test runner entry point
 */
int main(int argc, char** argv) {
    return seatest_testrunner(argc, argv, Test_AllTests, seatest_setup, seatest_teardown);
}
