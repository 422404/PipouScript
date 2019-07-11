#pragma once

#define TOK_SPACE ' '
#define TOK_TAB '\t'
#define TOK_EQ '='
#define TOK_PLUS '+'
#define TOK_MINUS '-'
#define TOK_TIMES '*'
#define TOK_DIV '/'
#define TOK_MOD '%'

typedef enum {
    TOKTYPE_SPACE,
    TOKTYPE_TAB,
    TOKTYPE_NEWLINE,
    TOKTYPE_EQ,
    TOKTYPE_PLUS,
    TOKTYPE_MINUS,
    TOKTYPE_TIMES,
    TOKTYPE_DIV,
    TOKTYPE_MOD,
    TOKTYPE_IDENT,
    TOKTYPE_STRING,
    TOKTYPE_INT,
    TOKTYPE_DOUBLE,
    TOKENS_NUMBER
} token_type_t;
