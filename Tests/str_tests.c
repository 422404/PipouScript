/**
 * @file str_tests.c
 * String tests
 */
#include "seatest.h"
#include "str.h"

static char * test_buf1 = "abcd";
static char * test_buf2 = "efgh";

void Test_AppendTest(void) {
    string * str1, * str2;

    str1 = Str_New(test_buf1);
    str2 = Str_New(test_buf2);
    Str_Append(str1, str2);
    assert_string_equal("abcdefgh", str1->c_str);
    Str_Free(str1);
    Str_Free(str2);
}

void Test_StringTests(void) {
    run_test(Test_AppendTest);
}
