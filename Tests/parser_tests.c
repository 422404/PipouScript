/**
 * @file parser_tests.c
 * Parser tests
 */
#include <string.h>
#include "Parser/include/parser.h"
#include "ast.h"
#include "seatest.h"

static char * test_buf1 = "abcd \"hello world!\" 1337 3.14159265";
static char * test_buf2 = "#abcd";
static char * test_buf3 = "#a:b:c:d";

// test_buf1
static ast_node_t expected1[] = {
    {NODE_IDENTIFIER, .as_ident  = {.value = "abcd"        }},
    {NODE_STRING,     .as_string = {.value = "hello world!"}},
    {NODE_INT,        .as_int    = {.value = 1337          }},
    {NODE_DOUBLE,     .as_double = {.value = 3.14159265    }},
};

void Test_ParseSimpleLitterals(void) {
    parser_t * parser;
    ast_node_t * node;

    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);

    node = Parser_ParseIdentifier(parser, false);
    assert_true(node != NULL);
    assert_int_equal(expected1[0].type, node->type);
    assert_string_equal(expected1[0].as_ident.value, node->as_ident.value);
    ASTNode_Free(node);

    node = Parser_ParseString(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[1].type, node->type);
    assert_string_equal(expected1[1].as_string.value, node->as_string.value);
    ASTNode_Free(node);

    node = Parser_ParseInt(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[2].type, node->type);
    assert_int_equal(expected1[2].as_int.value, node->as_int.value);
    ASTNode_Free(node);

    node = Parser_ParseDouble(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[3].type, node->type);
    assert_double_equal(expected1[3].as_double.value, node->as_double.value, 0.0);
    ASTNode_Free(node);

    Parser_Free(parser);
}

void Test_PushBackTokenList(void) {
    parser_t * parser;
    ast_node_t * node;

    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);

    for (size_t i = 0; i < 2; i++) {
        node = Parser_ParseInt(parser);
        assert_true(node == NULL);
    }
    node = Parser_ParseIdentifier(parser, false);
    assert_true(node != NULL);
    assert_int_equal(expected1[0].type, node->type);
    assert_string_equal(expected1[0].as_ident.value, node->as_ident.value);
    ASTNode_Free(node);

    Parser_Free(parser);
}

void Test_ParseObjFieldName(void) {
    parser_t * parser;
    ast_node_t * node, * ident;

    // test normal case where there is an identifier
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_OBJ_FIELD_NAME, node->type);
    assert_false(node->as_obj_field_name.is_msg_name);
    assert_int_equal(1, Vec_GetLength(node->as_obj_field_name.components));
    ident = Vec_GetAt(node->as_obj_field_name.components, 0);
    assert_int_equal(NODE_IDENTIFIER, ident->type);
    assert_string_equal("abcd", ident->as_ident.value);
    ASTNode_Free(node);
    Parser_Free(parser);

    // test normal case where there is a message name with 1 parameter
    parser = Parser_New(test_buf2, strlen(test_buf2), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_OBJ_FIELD_NAME, node->type);
    assert_true(node->as_obj_field_name.is_msg_name);
    assert_int_equal(1, Vec_GetLength(node->as_obj_field_name.components));
    ident = Vec_GetAt(node->as_obj_field_name.components, 0);
    assert_int_equal(NODE_IDENTIFIER, ident->type);
    assert_string_equal("abcd", ident->as_ident.value);
    ASTNode_Free(node);
    Parser_Free(parser);

    // test normal case where there is a message name with multiple parameters
    parser = Parser_New(test_buf3, strlen(test_buf3), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_OBJ_FIELD_NAME, node->type);
    assert_true(node->as_obj_field_name.is_msg_name);
    assert_int_equal(4, Vec_GetLength(node->as_obj_field_name.components));
    char * param_names[] = {"a", "b", "c", "d"};
    for (size_t i = 0; i < 4; i++) {
        ident = Vec_GetAt(node->as_obj_field_name.components, i);
        assert_int_equal(NODE_IDENTIFIER, ident->type);
        assert_string_equal(param_names[i], ident->as_ident.value);
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // test error case where there is no chars
    parser = Parser_New("    ", 4, NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node == NULL);
    Err_SetError(NULL);
    Parser_Free(parser);

    // test error case where there is only a '#'
    parser = Parser_New("#", 1, NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node == NULL);
    Err_SetError(NULL);
    Parser_Free(parser);

    // test error case where the message name end with ':'
    parser = Parser_New("#a:b:", 5, NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node == NULL);
    Err_SetError(NULL);
    Parser_Free(parser);
}

/**
 * Runs all the parser tests
 */
void Test_ParserTests(void) {
    run_test(Test_ParseSimpleLitterals);
    run_test(Test_PushBackTokenList);
    run_test(Test_ParseObjFieldName);
}
