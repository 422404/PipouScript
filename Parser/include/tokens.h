/**
 * @file tokens.h
 * (Auto-generated) Tokens declaration
 */
#pragma once

#define TOK_SPACE ' '
#define TOK_TAB '\t'
#define TOK_DQUOTE '"'
#define TOK_QUOTE '\''
#define TOK_LCBRACKET '{'
#define TOK_RCBRACKET '}'
#define TOK_LSBRACKET '['
#define TOK_RSBRACKET ']'
#define TOK_LPAREN '('
#define TOK_RPAREN ')'
#define TOK_COLON ':'
#define TOK_SEMICOLON ';'
#define TOK_PIPE '|'
#define TOK_CIRCUMFLEX '^'
#define TOK_EQUAL '='
#define TOK_DOT '.'
#define TOK_UNDERSCORE '_'
#define TOK_DOLLAR '$'
#define TOK_COMMA ','
#define TOK_AMP '&'
#define TOK_HASH '#'
#define TOK_LOWER '<'
#define TOK_GREATER '>'
#define TOK_PLUS '+'
#define TOK_MINUS '-'
#define TOK_STAR '*'
#define TOK_SLASH '/'
#define TOK_PERCENT '%'

typedef enum {
    TOKTYPE_SPACE,
    TOKTYPE_TAB,
    TOKTYPE_NEWLINE,
    TOKTYPE_DQUOTE,
    TOKTYPE_QUOTE,
    TOKTYPE_LCBRACKET,
    TOKTYPE_RCBRACKET,
    TOKTYPE_LSBRACKET,
    TOKTYPE_RSBRACKET,
    TOKTYPE_LPAREN,
    TOKTYPE_RPAREN,
    TOKTYPE_COLON,
    TOKTYPE_SEMICOLON,
    TOKTYPE_PIPE,
    TOKTYPE_CIRCUMFLEX,
    TOKTYPE_EQUAL,
    TOKTYPE_DOT,
    TOKTYPE_UNDERSCORE,
    TOKTYPE_DOLLAR,
    TOKTYPE_COMMA,
    TOKTYPE_AMP,
    TOKTYPE_HASH,
    TOKTYPE_LOWER,
    TOKTYPE_GREATER,
    TOKTYPE_PLUS,
    TOKTYPE_MINUS,
    TOKTYPE_STAR,
    TOKTYPE_SLASH,
    TOKTYPE_PERCENT,
    TOKTYPE_EQEQUAL,
    TOKTYPE_IDENT,
    TOKTYPE_STRING,
    TOKTYPE_INT,
    TOKTYPE_DOUBLE,
    TOKTYPE_COMMENT,
    TOKENS_NUMBER,
    TOKTYPE_NOTTOKEN
} token_type_t;

extern char * token_type_names[];
