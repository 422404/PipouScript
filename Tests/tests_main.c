/**
 * @file tests_main.c
 * Test runner entrypoint
 */
#include "seatest.h"
#include "vector_tests.h"
#include "iterator_tests.h"
#include "lexer_tests.h"

/**
 * Runs all tests
 */
void Test_AllTests(void) {
    Test_VectorTests();
    Test_IteratorTests();
    Test_LexerTests();
}

/**
 * Test runner entry point
 */
int main(int argc, char** argv) {
    return seatest_testrunner(argc, argv, Test_AllTests, seatest_setup, seatest_teardown);
}
