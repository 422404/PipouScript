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

static char * test_buf4 = "a: b + c";
static char * test_buf5 = "a: 1337";
static char * test_buf6 = "a:";
static char * test_buf7 = "a";

static char * test_buf8 = "abcd := bcde;";
static char * test_buf9 = "abcd := ;";
static char * test_buf10 = "abcd := efgh";

static char * test_buf11 = "[a]";
static char * test_buf12 = "[]";
static char * test_buf13 = "[";

static char * test_buf14 = "a.b";
static char * test_buf15 = "a[b]";
static char * test_buf16 = "a.#b:c";
static char * test_buf17 = "a.b.c";
static char * test_buf18 = "a[b].c";
static char * test_buf19 = "a[b].c[d]";
static char * test_buf20 = "a.";
static char * test_buf21 = "a[";
static char * test_buf22 = "a.#a:";
static char * test_buf23 = "a[b][c]";
static char * test_buf24 = "a[b][c].d";
static char * test_buf25 = "a[b].c[d][e]";

static char * test_buf26 = "a.b = abcd;";
static char * test_buf27 = "= abcd;";
static char * test_buf28 = "abcd = efgh";
static char * test_buf29 = "abcd = ";

static char * test_buf30 = "abcd";
static char * test_buf31 = "abcd;";
static char * test_buf32 = "^abcd";
static char * test_buf33 = "^ ";
static char * test_buf34 = "abcd :=";
static char * test_buf35 = "a.b.c.d = ";

static char * test_buf36 = "a || b";
static char * test_buf37 = "a || b || c";
static char * test_buf38 = "a && b";
static char * test_buf39 = "a && b || c";
static char * test_buf40 = "a + b + c || d";
static char * test_buf41 = "a + b - c + d";

static char * test_buf42 = "!hello";
static char * test_buf43 = "!    hello";

static char * test_buf44 = "(a + b)";
static char * test_buf45 = "(";
static char * test_buf46 = "(a";
static char * test_buf47 = "(a)";
static char * test_buf48 = "!(a + b)";

static char * test_buf49 = "[]";
static char * test_buf50 = "[1,2]";
static char * test_buf51 = "[1,2,]";
static char * test_buf52 = "[";
static char * test_buf53 = "[1";
static char * test_buf54 = "[1,";
static char * test_buf55 = "[,]";

static char * test_buf56 = "abcd a: b";
static char * test_buf57 = "abcd a";
static char * test_buf58 = "abcd a: \"wow!\" b: 1337";
static char * test_buf59 = "abcd a:";
static char * test_buf60 = "abcd a: 1337 b";

static char * test_buf61 = "{ ^abcd }";
static char * test_buf62 = "{ | abcd | ^abcd }";
static char * test_buf63 = "{ | abcd efgh | }";
static char * test_buf64 = "{ | }";
static char * test_buf65 = "{ | a }";
static char * test_buf66 = "{ }";

static char * test_buf67 = "a{}";
static char * test_buf68 = "a: b {}";
static char * test_buf69 = "a: b c: d {}";
static char * test_buf70 = "a: b c: d {\n^b + d\n}";
static char * test_buf71 = "a: b";
static char * test_buf72 = "a: b:";
static char * test_buf73 = "a: b c:";
static char * test_buf74 = "a: b c: d";
static char * test_buf75 = "a: b c: d {";

static char * test_buf76 = "{}";
static char * test_buf77 = "{a: 1337}";
static char * test_buf78 = "{a { expr }}";
static char * test_buf79 = "{a: b c: d { b + d }}";
static char * test_buf80 = "{a: 1337, c: 13.37}";
static char * test_buf81 = "{a: 1337, c: 13.37,}";
static char * test_buf82 = "{m: s g: p {s[p]}, a: 1337, c: 13.37}";

static char * test_buf83 = "1337abcd";

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
    ident = nanbox_to_pointer(Vec_GetAt(node->as_obj_field_name.components, 0));
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
    ident = nanbox_to_pointer(Vec_GetAt(node->as_obj_field_name.components, 0));
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
        ident = nanbox_to_pointer(Vec_GetAt(node->as_obj_field_name.components, i));
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
    Parser_Free(parser);

    // test error case where there is only a '#'
    parser = Parser_New("#", 1, NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // test error case where the message name end with ':'
    parser = Parser_New("#a:b:", 5, NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldName(parser);
    assert_true(node == NULL);
    Parser_Free(parser);
}

void Test_ParseDecl(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf8, strlen(test_buf8), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDecl(parser);
    assert_true(node != NULL);
    assert_string_equal("abcd", node->as_decl.lval->as_ident.value);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error/nominal case: that's not a decl
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDecl(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: no expression is supplied
    parser = Parser_New(test_buf9, strlen(test_buf9), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDecl(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: no semicolon at the end of the statement
    parser = Parser_New(test_buf10, strlen(test_buf10), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDecl(parser);
    assert_true(node == NULL);
    Parser_Free(parser);
}

void Test_ParseArrayAccess(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf11, strlen(test_buf11), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayAccess(parser);
    assert_true(node != NULL);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: no expression supplied
    parser = Parser_New(test_buf12, strlen(test_buf12), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayAccess(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: no closing square bracket supplied
    parser = Parser_New(test_buf13, strlen(test_buf13), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayAccess(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: no array access supplied
    parser = Parser_New("", strlen(""), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayAccess(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);
}

void Test_ParseDottedExpr(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_IDENTIFIER, node->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access field 'b' of object 'a'
    parser = Parser_New(test_buf14, strlen(test_buf14), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_OBJ_FIELD_NAME};
        for (size_t i = 0; i < 2; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
        printf("\n");
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access element at index 'b' in array 'a'
    parser = Parser_New(test_buf15, strlen(test_buf15), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS};
        for (size_t i = 0; i < 2; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access message '#b:c' of object 'a'
    parser = Parser_New(test_buf16, strlen(test_buf16), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_OBJ_FIELD_NAME};
        for (size_t i = 0; i < 2; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access field 'c' of object in field 'b' of object 'a'
    parser = Parser_New(test_buf17, strlen(test_buf17), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(3, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_OBJ_FIELD_NAME, NODE_OBJ_FIELD_NAME};
        for (size_t i = 0; i < 3; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access field 'c' of object at index 'b' in array 'a'
    parser = Parser_New(test_buf18, strlen(test_buf18), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(3, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS, NODE_OBJ_FIELD_NAME};
        for (size_t i = 0; i < 3; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: access element at index 'd' in array in field 'c' of object at index 'b' in array 'a'
    parser = Parser_New(test_buf19, strlen(test_buf19), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(4, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS, NODE_OBJ_FIELD_NAME, NODE_ARRAY_ACCESS};
        for (size_t i = 0; i < 4; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case
    parser = Parser_New(test_buf23, strlen(test_buf23), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(3, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS, NODE_ARRAY_ACCESS};
        for (size_t i = 0; i < 3; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case
    parser = Parser_New(test_buf24, strlen(test_buf24), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(4, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS, NODE_ARRAY_ACCESS, NODE_OBJ_FIELD_NAME};
        for (size_t i = 0; i < 4; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case
    parser = Parser_New(test_buf25, strlen(test_buf25), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(5, Vec_GetLength(node->as_dotted_expr.components));
    {
        ast_node_t * node2;
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARRAY_ACCESS, NODE_OBJ_FIELD_NAME, NODE_ARRAY_ACCESS, NODE_ARRAY_ACCESS};
        for (size_t i = 0; i < 5; i++) {
            node2 = nanbox_to_pointer(Vec_GetAt(node->as_dotted_expr.components, i));
            assert_int_equal(types[i], node2->type);
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: no field name after '.'
    parser = Parser_New(test_buf20, strlen(test_buf20), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: malformed array access
    parser = Parser_New(test_buf21, strlen(test_buf21), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: malformed message access
    parser = Parser_New(test_buf22, strlen(test_buf22), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseDottedExpr(parser);
    assert_true(node == NULL);
    Parser_Free(parser);
}

void Test_ParseAffect(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf26, strlen(test_buf26), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node != NULL);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error/nominal case: that's not an affect
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: the is an variable name but no '='
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: no variable name
    parser = Parser_New(test_buf27, strlen(test_buf27), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: no trailing semicolon
    parser = Parser_New(test_buf28, strlen(test_buf28), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    
    Parser_Free(parser);

    // error case: no expr
    parser = Parser_New(test_buf29, strlen(test_buf29), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAffect(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);
}

void Test_ParseStatement(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf30, strlen(test_buf30), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node != NULL);
    assert_true(node->as_statement.is_local_return);
    assert_false(node->as_statement.is_mod_statement);
    assert_true(node->as_statement.is_return_expr);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: cannot return from module
    parser = Parser_New(test_buf30, strlen(test_buf30), NULL, true);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, true);
    assert_true(node == NULL);
    Parser_Free(parser);

    // nominal case
    parser = Parser_New(test_buf31, strlen(test_buf31), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node != NULL);
    assert_false(node->as_statement.is_local_return);
    assert_true(node->as_statement.is_mod_statement);
    assert_false(node->as_statement.is_return_expr);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case
    parser = Parser_New(test_buf32, strlen(test_buf32), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node != NULL);
    assert_false(node->as_statement.is_local_return);
    assert_false(node->as_statement.is_mod_statement);
    assert_true(node->as_statement.is_return_expr);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: cannot return from module
    parser = Parser_New(test_buf32, strlen(test_buf32), NULL, true);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, true);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: no expr after '^'
    parser = Parser_New(test_buf33, strlen(test_buf33), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: malformed decl
    parser = Parser_New(test_buf34, strlen(test_buf34), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node == NULL);
    Parser_Free(parser);

    // error case: malformed affect
    parser = Parser_New(test_buf35, strlen(test_buf35), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseStatement(parser, false);
    assert_true(node == NULL);
    Parser_Free(parser);
}

void Test_ParseBinaryExpr(void) {
    parser_t * parser;
    ast_node_t * node;
    ast_node_t * val;

    // nominal case: simple binary OR
    parser = Parser_New(test_buf36, strlen(test_buf36), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_OR_EXPR, node->type);
    assert_int_equal(TOKTYPE_PIPEPIPE, node->as_expr.op);
    assert_int_equal(2, Vec_GetLength(node->as_expr.values));
    for (size_t i = 0; i < 2; i++) {
        val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
        assert_int_equal(NODE_IDENTIFIER, val->type);
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: simple ternary OR
    parser = Parser_New(test_buf37, strlen(test_buf37), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_OR_EXPR, node->type);
    assert_int_equal(TOKTYPE_PIPEPIPE, node->as_expr.op);
    assert_int_equal(3, Vec_GetLength(node->as_expr.values));
    for (size_t i = 0; i < 3; i++) {
        val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
        assert_int_equal(NODE_IDENTIFIER, val->type);
    }
    ASTNode_Free(node);
    Parser_Free(parser);
    
    // nominal case: simple binary AND
    parser = Parser_New(test_buf38, strlen(test_buf38), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_AND_EXPR, node->type);
    assert_int_equal(TOKTYPE_AMPAMP, node->as_expr.op);
    assert_int_equal(2, Vec_GetLength(node->as_expr.values));
    for (size_t i = 0; i < 2; i++) {
        val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
        assert_int_equal(NODE_IDENTIFIER, val->type);
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: binary OR of a binary AND and a variable
    parser = Parser_New(test_buf39, strlen(test_buf39), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_OR_EXPR, node->type);
    assert_int_equal(TOKTYPE_PIPEPIPE, node->as_expr.op);
    assert_int_equal(2, Vec_GetLength(node->as_expr.values));
    {
        ast_node_type_t types[] = {NODE_AND_EXPR, NODE_IDENTIFIER};
        ast_node_t * val2;
        for (size_t i = 0; i < 2; i++) {
            val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
            assert_int_equal(types[i], val->type);
            if (val->type == NODE_AND_EXPR) {
                for (size_t j = 0; j < 2; j++) {
                    val2 = nanbox_to_pointer(Vec_GetAt(val->as_expr.values, j));
                    assert_int_equal(NODE_IDENTIFIER, val2->type);
                }
            }
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: binary OR of a ternary addition of variables and a variable
    parser = Parser_New(test_buf40, strlen(test_buf40), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_OR_EXPR, node->type);
    assert_int_equal(TOKTYPE_PIPEPIPE, node->as_expr.op);
    assert_int_equal(2, Vec_GetLength(node->as_expr.values));
    {
        ast_node_type_t types[] = {NODE_ARITH_EXPR, NODE_IDENTIFIER};
        ast_node_t * val2;
        for (size_t i = 0; i < 2; i++) {
            val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
            assert_int_equal(types[i], val->type);
            if (val->type == NODE_ARITH_EXPR) {
                assert_int_equal(3, Vec_GetLength(val->as_expr.values));
                for (size_t j = 0; j < 3; j++) {
                    val2 = nanbox_to_pointer(Vec_GetAt(val->as_expr.values, j));
                    assert_int_equal(NODE_IDENTIFIER, val2->type);
                }
            }
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: addition of a variable and a substraction of a variable
    //               and an addition of two variables
    parser = Parser_New(test_buf41, strlen(test_buf41), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBinaryExpr(parser, NODE_OR_EXPR);
    assert_true(node != NULL);
    assert_int_equal(NODE_ARITH_EXPR, node->type);
    assert_int_equal(TOKTYPE_PLUS, node->as_expr.op);
    assert_int_equal(2, Vec_GetLength(node->as_expr.values));
    {
        ast_node_type_t types[] = {NODE_IDENTIFIER, NODE_ARITH_EXPR};
        ast_node_t * val2, * val3;
        for (size_t i = 0; i < 2; i++) {
            val = nanbox_to_pointer(Vec_GetAt(node->as_expr.values, i));
            assert_int_equal(types[i], val->type);
            if (val->type == NODE_ARITH_EXPR) {
                assert_int_equal(2, Vec_GetLength(val->as_expr.values));
                for (size_t j = 0; j < 2; j++) {
                    val2 = nanbox_to_pointer(Vec_GetAt(val->as_expr.values, j));
                    assert_int_equal(types[j], val2->type);
                    if (val2->type == NODE_ARITH_EXPR) {
                        assert_int_equal(2, Vec_GetLength(val2->as_expr.values));
                        for (size_t k = 0; k < 2; k++) {
                            val3 = nanbox_to_pointer(Vec_GetAt(val2->as_expr.values, k));
                            assert_int_equal(NODE_IDENTIFIER, val3->type);
                        }
                    }
                }
            }
        }
    }
    ASTNode_Free(node);
    Parser_Free(parser);

    /// @todo test error cases
}

void Test_ParseUnaryExpr(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf42, strlen(test_buf42), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseUnaryExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(TOKTYPE_EXCL, node->as_expr.op);
    ASTNode_Free(node);
    Parser_Free(parser);
    
    // nominal case
    parser = Parser_New(test_buf43, strlen(test_buf43), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseUnaryExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(TOKTYPE_EXCL, node->as_expr.op);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error/nominal case: no unary_expr to parse
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseUnaryExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);
}

void Test_ParseAtomExpr(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: there is an unary_expr
    parser = Parser_New(test_buf42, strlen(test_buf42), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_UNARY_EXPR, node->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: there is a litteral
    /// @todo todo!

    // nominal case: there is an arith_expr
    parser = Parser_New(test_buf44, strlen(test_buf44), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_ARITH_EXPR, node->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: there is a expression
    parser = Parser_New(test_buf47, strlen(test_buf47), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_IDENTIFIER, node->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: not operation on a sum
    parser = Parser_New(test_buf48, strlen(test_buf48), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_UNARY_EXPR, node->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: no expression after '('
    parser = Parser_New(test_buf45, strlen(test_buf45), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    
    Parser_Free(parser);

    // error case: no ')' after expression
    parser = Parser_New(test_buf46, strlen(test_buf46), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    
    Parser_Free(parser);

    // error/nominal case: there is nothing to parse
    parser = Parser_New(" ", strlen(" "), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseAtomExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);
}

void Test_ParseLitteralExpr(void) {
    parser_t * parser;
    ast_node_t * node;

    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    // we use it so we can keep using test_buf1 but the test
    // subject isn't about Parser_ParseIdentifier()
    node = Parser_ParseIdentifier(parser, false);
    assert_true(node != NULL);
    ASTNode_Free(node);

    // nominal case: string
    node = Parser_ParseLitteralExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[1].type, node->type);
    assert_string_equal(expected1[1].as_string.value, node->as_string.value);
    ASTNode_Free(node);

    // nominal case: integer
    node = Parser_ParseLitteralExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[2].type, node->type);
    assert_int_equal(expected1[2].as_int.value, node->as_int.value);
    ASTNode_Free(node);

    // nominal case: double
    node = Parser_ParseLitteralExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(expected1[3].type, node->type);
    assert_double_equal(expected1[3].as_double.value, node->as_double.value, 0.0);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error/nominal case: nothing to parse
    parser = Parser_New(" ", strlen(" "), NULL, false);
    assert_true(node != NULL);
    node = Parser_ParseLitteralExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    /// @todo error cases
}

void Test_ParseArrayLitteral(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: empty array
    parser = Parser_New(test_buf49, strlen(test_buf49), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node != NULL);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: array with 2 items
    parser = Parser_New(test_buf50, strlen(test_buf50), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_array_litteral.items));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: array with 2 items and a trailing comma
    parser = Parser_New(test_buf51, strlen(test_buf51), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_array_litteral.items));
    ASTNode_Free(node);
    Parser_Free(parser);

    // error/nominal case: no array_litteral
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: only a '['
    parser = Parser_New(test_buf52, strlen(test_buf52), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: one item and no ']'
    parser = Parser_New(test_buf53, strlen(test_buf53), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: one item, a trailing comma and no ']'
    parser = Parser_New(test_buf54, strlen(test_buf54), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: only a comma between the square brackets
    parser = Parser_New(test_buf55, strlen(test_buf55), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseArrayLitteral(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);
}

void Test_ParseMsgPassExpr(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: message with one parameter
    parser = Parser_New(test_buf56, strlen(test_buf56), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_MSG_PASS_EXPR, node->type);
    assert_int_equal(3, Vec_GetLength(node->as_msg_pass_expr.components));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: message with no parameters
    parser = Parser_New(test_buf57, strlen(test_buf57), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_MSG_PASS_EXPR, node->type);
    assert_int_equal(2, Vec_GetLength(node->as_msg_pass_expr.components));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: message with two parameters
    parser = Parser_New(test_buf58, strlen(test_buf58), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node != NULL);
    assert_int_equal(NODE_MSG_PASS_EXPR, node->type);
    assert_int_equal(5, Vec_GetLength(node->as_msg_pass_expr.components));
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: value of parameter not supplied
    parser = Parser_New(test_buf59, strlen(test_buf59), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: trailing ident
    parser = Parser_New(test_buf60, strlen(test_buf60), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: no spacing before message name
    parser = Parser_New(test_buf83, strlen(test_buf83), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseMsgPassExpr(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);
}

void Test_ParseBlock(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: block with return statement
    parser = Parser_New(test_buf61, strlen(test_buf61), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node != NULL);
    assert_int_equal(0, Vec_GetLength(node->as_block.params));
    assert_int_equal(1, Vec_GetLength(node->as_block.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: block with one parameter and return statement
    parser = Parser_New(test_buf62, strlen(test_buf62), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node != NULL);
    assert_int_equal(1, Vec_GetLength(node->as_block.params));
    assert_int_equal(1, Vec_GetLength(node->as_block.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: block with two parameters and no statement
    parser = Parser_New(test_buf63, strlen(test_buf63), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_block.params));
    assert_int_equal(0, Vec_GetLength(node->as_block.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: block with a '|'
    parser = Parser_New(test_buf64, strlen(test_buf64), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: block with a '|' and a parameter
    parser = Parser_New(test_buf65, strlen(test_buf65), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error/nominal case: empty curly brackets interpreted as an object, so no result
    parser = Parser_New(test_buf66, strlen(test_buf66), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error/nominal case: no block to parse
    parser = Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseBlock(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);
}

void Test_ParseObjMsgDef(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: a message with no params or statements
    parser= Parser_New(test_buf67, strlen(test_buf67), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node != NULL);
    assert_int_equal(1, Vec_GetLength(node->as_obj_msg_def.selector));
    assert_int_equal(0, Vec_GetLength(node->as_obj_msg_def.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: a message with one param and no statements
    parser= Parser_New(test_buf68, strlen(test_buf68), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_obj_msg_def.selector));
    assert_int_equal(0, Vec_GetLength(node->as_obj_msg_def.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: a message with two param and no statements
    parser= Parser_New(test_buf69, strlen(test_buf69), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node != NULL);
    assert_int_equal(4, Vec_GetLength(node->as_obj_msg_def.selector));
    assert_int_equal(0, Vec_GetLength(node->as_obj_msg_def.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: a message with two param and one statement
    parser= Parser_New(test_buf70, strlen(test_buf70), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node != NULL);
    assert_int_equal(4, Vec_GetLength(node->as_obj_msg_def.selector));
    assert_int_equal(1, Vec_GetLength(node->as_obj_msg_def.statements));
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: an ident but no '{' or ':'
    parser= Parser_New(test_buf1, strlen(test_buf1), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error/nominal case: not an obj_msg_def but maybe an obj_field_init
    parser= Parser_New(test_buf71, strlen(test_buf71), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: ':' istead of '{' after parameter name, so like previous test
    parser= Parser_New(test_buf72, strlen(test_buf72), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) == NULL);
    Parser_Free(parser);

    // error case: no name for the second param
    parser= Parser_New(test_buf73, strlen(test_buf73), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: no '{' after message signature
    parser= Parser_New(test_buf74, strlen(test_buf74), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: no '}' after statements
    parser= Parser_New(test_buf75, strlen(test_buf75), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjMsgDef(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    /// @todo more tests
}

void Test_ParseObjFieldInit(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case
    parser = Parser_New(test_buf4, strlen(test_buf4), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldInit(parser);
    assert_true(node != NULL);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case 2
    parser = Parser_New(test_buf5, strlen(test_buf5), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldInit(parser);
    assert_true(node != NULL);
    ASTNode_Free(node);
    Parser_Free(parser);

    // error case: no expr after ':'
    parser = Parser_New(test_buf6, strlen(test_buf6), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldInit(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);

    // error case: no ':' after ident
    parser = Parser_New(test_buf7, strlen(test_buf7), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjFieldInit(parser);
    assert_true(node == NULL);
    assert_true(Parser_GetError(parser) != NULL);
    Parser_Free(parser);
}

void Test_ParseObjLitteral(void) {
    parser_t * parser;
    ast_node_t * node;

    // nominal case: obj_litteral with no members
    parser = Parser_New(test_buf76, strlen(test_buf76), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(0, Vec_GetLength(node->as_obj_litteral.obj_fields));
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral one field
    parser = Parser_New(test_buf77, strlen(test_buf77), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(1, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral with one message
    parser = Parser_New(test_buf78, strlen(test_buf78), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(1, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_MSG_DEF, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral with one message with multiple parameters
    parser = Parser_New(test_buf79, strlen(test_buf79), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(1, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_MSG_DEF, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral with two fields
    parser = Parser_New(test_buf80, strlen(test_buf80), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 1)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral with two fields and a trailing ','
    parser = Parser_New(test_buf81, strlen(test_buf81), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(2, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 1)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    // nominal case: obj_litteral with a multiple parameters message and two fields and a trailing
    parser = Parser_New(test_buf82, strlen(test_buf82), NULL, false);
    assert_true(parser != NULL);
    node = Parser_ParseObjLitteral(parser);
    assert_true(node != NULL);
    assert_int_equal(3, Vec_GetLength(node->as_obj_litteral.obj_fields));
    assert_int_equal(NODE_OBJ_MSG_DEF, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 0)))->type);
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 1)))->type);
    assert_int_equal(NODE_OBJ_FIELD_INIT, ((ast_node_t *)nanbox_to_pointer(
            Vec_GetAt(node->as_obj_litteral.obj_fields, 2)))->type);
    ASTNode_Free(node);
    Parser_Free(parser);

    /// @todo error cases
}

/**
 * Runs all the parser tests
 */
void Test_ParserTests(void) {
    test_fixture_start();
    run_test(Test_ParseSimpleLitterals);
    run_test(Test_PushBackTokenList);
    run_test(Test_ParseObjFieldName);
    run_test(Test_ParseDecl);
    run_test(Test_ParseArrayAccess);
    run_test(Test_ParseDottedExpr);
    run_test(Test_ParseAffect);
    run_test(Test_ParseStatement);
    run_test(Test_ParseBinaryExpr);
    run_test(Test_ParseUnaryExpr);
    run_test(Test_ParseAtomExpr);
    run_test(Test_ParseLitteralExpr);
    run_test(Test_ParseArrayLitteral);
    run_test(Test_ParseMsgPassExpr);
    run_test(Test_ParseBlock);
    run_test(Test_ParseObjMsgDef);
    run_test(Test_ParseObjFieldInit);
    run_test(Test_ParseObjLitteral);
    test_fixture_end();
}
