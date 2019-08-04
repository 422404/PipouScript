#include <string.h>
#include "seatest.h"
#include "lexer.h"
#include "token.h"
#include "tokens.h"

static char * test_buf1 = "{]\n)% ";
static char * test_buf2 = "    [ \n%";
static char * test_buf3 = "*";
static char * test_buf4 = "% \"abcd efgh\\nwow!\" *";
static char * test_buf5 = "\"   ";
static char * test_buf6 = "abcd \"abcd\" // comment \"not a string\"";
static char * test_buf7 = "10 1337 1234.0 456.7e89";
static char * test_buf8 = ":= == : =";

// test_buf1
token_t expected_tokens1[] = {
    {TOKTYPE_LCBRACKET,  {{1, 1, NULL}, {1, 1, NULL}}, NULL},
    {TOKTYPE_RSBRACKET,  {{1, 2, NULL}, {1, 2, NULL}}, NULL},
    {TOKTYPE_NEWLINE,    {{1, 3, NULL}, {1, 3, NULL}}, NULL},
    {TOKTYPE_RPAREN,     {{2, 1, NULL}, {2, 1, NULL}}, NULL},
    {TOKTYPE_PERCENT,    {{2, 2, NULL}, {2, 2, NULL}}, NULL},
    {TOKTYPE_SPACE,      {{2, 3, NULL}, {2, 3, NULL}}, NULL}
};

// test_buf2
token_t expected_tokens2[] = {
    {TOKTYPE_LSBRACKET,  {{1, 5, NULL}, {1, 5, NULL}}, NULL},
    {TOKTYPE_PERCENT,    {{2, 1, NULL}, {2, 1, NULL}}, NULL}
};

// test_buf4
token_t expected_tokens3[] = {
    {TOKTYPE_PERCENT,    {{1,  1, NULL}, {1,  1, NULL}}, NULL                  },
    {TOKTYPE_STRING,     {{1,  3, NULL}, {1, 19, NULL}}, "\"abcd efgh\\nwow!\""},
    {TOKTYPE_STAR,       {{1, 21, NULL}, {1, 21, NULL}}, NULL                  }
};

// test_buf6
token_t expected_tokens4[] = {
    {TOKTYPE_IDENT,      {{1,  1, NULL}, {1,  4, NULL}}, "abcd"                       },
    {TOKTYPE_STRING,     {{1,  6, NULL}, {1, 11, NULL}}, "\"abcd\""                   },
    {TOKTYPE_COMMENT,    {{1, 13, NULL}, {1, 37, NULL}}, "// comment \"not a string\""}
};

// test_buf7
token_t expected_tokens5[] = {
    {TOKTYPE_INT,      {{1,  1, NULL}, {1,  2, NULL}}, NULL},
    {TOKTYPE_INT,      {{1,  4, NULL}, {1,  7, NULL}}, NULL},
    {TOKTYPE_INT,      {{1,  9, NULL}, {1, 14, NULL}}, NULL},
    {TOKTYPE_INT,      {{1, 15, NULL}, {1, 23, NULL}}, NULL}
};

// test_buf8
token_t expected_tokens6[] = {
    {TOKTYPE_COLEQUAL, {{1, 1, NULL}, {1, 2, NULL}}, NULL},
    {TOKTYPE_EQEQUAL,  {{1, 4, NULL}, {1, 5, NULL}}, NULL},
    {TOKTYPE_COLON,    {{1, 7, NULL}, {1, 7, NULL}}, NULL},
    {TOKTYPE_EQUAL,    {{1, 9, NULL}, {1, 9, NULL}}, NULL}
};

static inline void MatchTokens(lexer_t * lexer, token_t expected_tokens[], size_t expected_tokens_length,
                               bool preserve_whitespaces, bool preserve_comments) {
    token_t * token;

    for (size_t i = 0; i < expected_tokens_length; i++) {
        token = Lex_NextToken(lexer, preserve_whitespaces, preserve_comments);
        assert_true(token != NULL);
        if (token) {
            assert_int_equal(expected_tokens[i].type,            token->type);
            assert_int_equal(expected_tokens[i].span.start.col,  token->span.start.col);
            assert_int_equal(expected_tokens[i].span.start.line, token->span.start.line);
            assert_int_equal(expected_tokens[i].span.end.col,    token->span.end.col);
            assert_int_equal(expected_tokens[i].span.end.line,   token->span.end.line);
            Token_Free(token);
        } else {
            printf("Error with i=%ld", i);
        }
    }
}

/**
 * Tests the allocation of a lexer
 */
void Test_LexerCreation(void) {
    lexer_t * lexer;

    lexer = Lex_New(test_buf1, strlen(test_buf1), NULL);
    assert_true(lexer != NULL);
    Lex_Free(lexer);
}

void Test_LexerBasicTokenRecognition(void) {
    lexer_t * lexer;

    lexer = Lex_New(test_buf1, strlen(test_buf1), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens1, 6, true, false);
    Lex_Free(lexer);
}

void Test_LexerBasicSkipWhitespaces(void) {
    lexer_t * lexer;

    lexer = Lex_New(test_buf2, strlen(test_buf2), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens2, 2, false, false);
    Lex_Free(lexer);
}

void Test_LexerEOF(void) {
    lexer_t * lexer;
    token_t * token;

    lexer = Lex_New(test_buf3, strlen(test_buf3), NULL);
    assert_true(lexer != NULL);
    token = Lex_NextToken(lexer, false, false);
    assert_true(token != NULL);
    assert_int_equal(TOKTYPE_STAR, token->type);
    // there's only one token so we have reached buffer end
    assert_int_equal(LEX_EOF, Lex_GetStatus(lexer));
    assert_true(token->span.start.filename == NULL);
    Token_Free(token);
    token = Lex_NextToken(lexer, false, false);
    // no token can be extracted
    assert_true(token == NULL);
    // lexer status keeps beeing LEX_EOF
    assert_int_equal(LEX_EOF, Lex_GetStatus(lexer));
    Lex_Free(lexer);
}

void Test_LexerCompoundTokens(void) {
    lexer_t * lexer;

    lexer = Lex_New(test_buf4, strlen(test_buf4), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens3, 3, false, false);
    Lex_Free(lexer);

    lexer = Lex_New(test_buf6, strlen(test_buf6), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens4, 3, false, true);
    Lex_Free(lexer);

    lexer = Lex_New(test_buf7, strlen(test_buf7), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens5, 2, false, false);
    Lex_Free(lexer);
}

void Test_LexerUnterminatedStringError(void) {
    lexer_t * lexer;
    token_t * token;

    lexer = Lex_New(test_buf5, strlen(test_buf5), NULL);
    assert_true(lexer != NULL);
    token = Lex_NextToken(lexer, false, false);
    assert_true(token == NULL);
    assert_int_equal(LEX_ERROR, Lex_GetStatus(lexer));
    Lex_Free(lexer);
    Err_SetError(NULL);
}

void Test_LexerMulticharOperators(void) {
    lexer_t * lexer;

    lexer = Lex_New(test_buf8, strlen(test_buf8), NULL);
    assert_true(lexer != NULL);
    MatchTokens(lexer, expected_tokens6, 4, false, false);
    Lex_Free(lexer);
}

/**
 * Runs all lexer tests
 */
void Test_LexerTests(void) {
    test_fixture_start();
    run_test(Test_LexerCreation);
    run_test(Test_LexerBasicTokenRecognition);
    run_test(Test_LexerBasicSkipWhitespaces);
    run_test(Test_LexerEOF);
    run_test(Test_LexerCompoundTokens);
    run_test(Test_LexerUnterminatedStringError);
    run_test(Test_LexerMulticharOperators);
    test_fixture_end();
}
