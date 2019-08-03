/**
 * @file lexer.c
 * Input lexical analysis
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "misc.h"
#include "token.h"
#include "lexer.h"
/**
 * @private
 */
static bool Lex_InBufferBounds(lexer_t * lexer, char * char_ptr) {
    return char_ptr >= lexer->buffer
            && char_ptr < lexer->buffer + lexer->buffer_length;
}

/**
 * @private
 */
static bool Lex_NextCharIs(lexer_t * lexer, char c) {
    return Lex_InBufferBounds(lexer, lexer->current_char + 1)
            && *(lexer->current_char + 1) == c;
}

/**
 * @private
 */
static bool Lex_PrevCharIs(lexer_t * lexer, char c) {
    return Lex_InBufferBounds(lexer, lexer->current_char - 1)
            && *(lexer->current_char - 1) == c;
}

/**
 * Allocates a new lexer
 * @param[in] buffer   The buffer where tokens will be extracted
 * @param     length   Length of the input buffer
 * @param[in] filename Name of the file that contains the code (NULL if in REPL mode)
 * @returns          A pointer to the newly allocated lexer
 */
lexer_t * Lex_New(char * buffer, size_t length, char * filename) {
    lexer_t * lexer = NULL;

    if (buffer == NULL) Err_Throw(Err_New("NULL pointer to lexer chars buffer"));

    lexer = (lexer_t *)malloc(sizeof(lexer_t));
    if (lexer) {
        loc_t pos = {1, 1, filename};
        lexer->buffer = buffer;
        lexer->buffer_length = length;
        lexer->current_char = buffer;
        lexer->pos = pos;
        lexer->filename = filename;
        lexer->status = length != 0 ? LEX_OK : LEX_EOF;
    } else {
        Err_Throw(Err_New("Cannot allocate lexer"));
    }
    return lexer;
}

/**
 * Frees an allocated lexer.
 * Calling the function don't free the input buffer or the
 * extracted tokens
 * @param[in] lexer A pointer to the lexer to be freed
 */
void Lex_Free(lexer_t * lexer) {
    if (lexer) free(lexer);
    else Err_Throw(Err_New("NULL pointer to lexer"));
}

/**
 * Increments the current_char pointer and updates the pos and status fields
 * @private
 * @param[in] lexer The lexer to increment
 */
static void Lex_IncrementCurrentChar(lexer_t * lexer) {
    if (lexer->current_char < lexer->buffer + lexer->buffer_length) {
        lexer->current_char++;
        if (lexer->current_char == lexer->buffer + lexer->buffer_length) {
            lexer->status = LEX_EOF;
        } else {
            if (Lex_PrevCharIs(lexer, '\n')
                    || (Lex_PrevCharIs(lexer, '\r')
                        && *lexer->current_char != '\n')) {
                lexer->pos.line++;
                lexer->pos.col = 1;
            } else {
                lexer->pos.col++;
            }
        }
    } else {
        lexer->status = LEX_ERROR;
    }
}

/**
 * Try to parse an operator that span multiple chars
 * @todo tests
 * @retval True if an operator has been parse
 * @retval False if not
 */
static bool Lex_ParseMulticharOperator(lexer_t * lexer, token_type_t * token_type, loc_t * token_end) {
    struct token_transform {
        token_type_t current_token_type;
        char         next_char;
        token_type_t transform_to;
    } transforms[] = {
        {TOKTYPE_EQUAL,   TOK_EQUAL, TOKTYPE_EQEQUAL },
        {TOKTYPE_EXCL,    TOK_EQUAL, TOKTYPE_NOTEQUAL},
        {TOKTYPE_GREATER, TOK_EQUAL, TOKTYPE_GEQUAL  },
        {TOKTYPE_LOWER,   TOK_EQUAL, TOKTYPE_LEQUAL  },
        {TOKTYPE_COLON,   TOK_EQUAL, TOKTYPE_COLEQUAL},
        {TOKTYPE_PIPE,    TOK_PIPE,  TOKTYPE_PIPEPIPE},
        {TOKTYPE_AMP,     TOK_AMP,   TOKTYPE_AMPAMP  }
    };
    bool success = false;

    for (size_t i = 0; i < 7 && !success; i++) {
        if (*token_type == transforms[i].current_token_type
                && Lex_NextCharIs(lexer, transforms[i].next_char)) {
            *token_type = transforms[i].transform_to;
            Lex_IncrementCurrentChar(lexer);
            token_end->col++;
            success =  true;
        }
    }

    return success;
}

/**
 * Try to parse a simple token
 * @private
 * @param[in] lexer The lexer used to extract the token
 * @retval A pointer to an allocated token if one is exctracted
 * @retval NULL is none can be found
 */
static token_t * Lex_ParseSimpleToken(lexer_t * lexer) {
    token_t * token = NULL;
    token_type_t token_type = TOKTYPE_NOTTOKEN;
    loc_t token_start, token_end;
    span_t token_span;
    token_start = token_end = lexer->pos;

    if (lexer->status != LEX_EOF && lexer->status != LEX_ERROR) {
        char c = *lexer->current_char;
        switch (c) {
            case TOK_SPACE:
                token_type = TOKTYPE_SPACE;
                break;
            case TOK_TAB:
                token_type = TOKTYPE_TAB;
                break;
            case TOK_DQUOTE:
                token_type = TOKTYPE_DQUOTE;
                break;
            case TOK_QUOTE:
                token_type = TOKTYPE_QUOTE;
                break;
            case TOK_LCBRACKET:
                token_type = TOKTYPE_LCBRACKET;
                break;
            case TOK_RCBRACKET:
                token_type = TOKTYPE_RCBRACKET;
                break;
            case TOK_LSBRACKET:
                token_type = TOKTYPE_LSBRACKET;
                break;
            case TOK_RSBRACKET:
                token_type = TOKTYPE_RSBRACKET;
                break;
            case TOK_LPAREN:
                token_type = TOKTYPE_LPAREN;
                break;
            case TOK_RPAREN:
                token_type = TOKTYPE_RPAREN;
                break;
            case TOK_COLON:
                token_type = TOKTYPE_COLON;
                break;
            case TOK_SEMICOLON:
                token_type = TOKTYPE_SEMICOLON;
                break;
            case TOK_PIPE:
                token_type = TOKTYPE_PIPE;
                break;
            case TOK_CIRCUMFLEX:
                token_type = TOKTYPE_CIRCUMFLEX;
                break;
            case TOK_EQUAL:
                token_type = TOKTYPE_EQUAL;
                break;
            case TOK_DOT:
                token_type = TOKTYPE_DOT;
                break;
            case TOK_UNDERSCORE:
                token_type = TOKTYPE_UNDERSCORE;
                break;
            case TOK_DOLLAR:
                token_type = TOKTYPE_DOLLAR;
                break;
            case TOK_COMMA:
                token_type = TOKTYPE_COMMA;
                break;
            case TOK_AMP:
                token_type = TOKTYPE_AMP;
                break;
            case TOK_HASH:
                token_type = TOKTYPE_HASH;
                break;
            case TOK_LOWER:
                token_type = TOKTYPE_LOWER;
                break;
            case TOK_GREATER:
                token_type = TOKTYPE_GREATER;
                break;
            case TOK_PLUS:
                token_type = TOKTYPE_PLUS;
                break;
            case TOK_MINUS:
                token_type = TOKTYPE_MINUS;
                break;
            case TOK_STAR:
                token_type = TOKTYPE_STAR;
                break;
            case TOK_SLASH:
                token_type = TOKTYPE_SLASH;
                break;
            case TOK_PERCENT:
                token_type = TOKTYPE_PERCENT;
                break;
            case TOK_EXCL:
                token_type = TOKTYPE_EXCL;
                break;
        }

        if (c == '\n' || c == '\r') {
            token_type = TOKTYPE_NEWLINE;
            if (c == '\r' && Lex_NextCharIs(lexer, '\n')) {
                Lex_IncrementCurrentChar(lexer);
                token_end.col++;
            }
        }
        Lex_ParseMulticharOperator(lexer, &token_type, &token_end);

        if (token_type != TOKTYPE_NOTTOKEN) { // if matched
            Lex_IncrementCurrentChar(lexer);
            token_span.start = token_start;
            token_span.end   = token_end;
            token = Token_New(token_type, token_span);
        } else {
            lexer->status = LEX_ERROR;
            char * buf = malloc(100);
            /// @todo Add filename indication
            snprintf(buf, 100, "Unrecognized token '%c' (%ld:%ld)", c, lexer->pos.line, lexer->pos.col);
            Err_SetError(Err_New(buf));
            free(buf);
        }
    }
    return token;
}

/**
 * Try to parse a string
 * If the begining of a string is found the absence of its termination will be treated as
 * an error and the lexer will not fall back to extract a simple token
 * Updates lexer state in case of a match
 * @private
 * @param[in]  lexer The lexer used to extract the string
 * @returns          Whether a string can be extracted
 */
static bool Lex_TryParseString(lexer_t * lexer) {
    bool fullmatch = false;
    char * char_ptr = lexer->current_char;

    if (*char_ptr == '"') {
        while (++char_ptr && Lex_InBufferBounds(lexer, char_ptr)
                && (*char_ptr != '"' || (*(char_ptr - 1) == '\\' && *char_ptr == '"')));
        
        if (Lex_InBufferBounds(lexer, char_ptr) && *char_ptr == '"') {
            while (lexer->current_char != char_ptr) Lex_IncrementCurrentChar(lexer);
            fullmatch = true;
        } else {
            lexer->status = LEX_ERROR;
            /// @todo Add line, col and filename indication
            Err_SetError(Err_New("String not terminated"));
        }
    }
    return fullmatch;
}

/**
 * Try to parse an identifier
 * Updates lexer state in case of a match
 * @private
 * @param[in]  lexer The lexer used to extract the identifier
 * @returns          Whether an identifier can be extracted
 */
static bool Lex_TryParseIdentifier(lexer_t * lexer) {
    bool fullmatch = false;
    char * char_ptr = lexer->current_char;

    if ((*char_ptr >= 'a' && *char_ptr <= 'z')
            || (*char_ptr >= 'A' && *char_ptr <= 'Z')
            || *char_ptr == '_'
            || *char_ptr == '$'
            || *char_ptr >= 0xc0) {
        while (++char_ptr && Lex_InBufferBounds(lexer, char_ptr)
                && ((*char_ptr >= 'a' && *char_ptr <= 'z')
                    || (*char_ptr >= 'A' && *char_ptr <= 'Z')
                    || (*char_ptr >= '0' && *char_ptr <= '9')
                    || *char_ptr == '_' || *char_ptr == '$'
                    || *char_ptr >= 0xc0)) Lex_IncrementCurrentChar(lexer);
        fullmatch = true;
    }
    return fullmatch;
}

/**
 * Try to parse a comment
 * Updates lexer state in case of a match
 * @private
 * @param[in]  lexer The lexer used to extract the comment
 * @returns          Whether a comment can be extracted
 */
static bool Lex_TryParseComment(lexer_t * lexer) {
    bool fullmatch = false;
    char * char_ptr = lexer->current_char;

    if (*char_ptr == '/' && Lex_NextCharIs(lexer, '/')) {
        while (++char_ptr && Lex_InBufferBounds(lexer, char_ptr)
                && *char_ptr != '\n' && *char_ptr != '\r') Lex_IncrementCurrentChar(lexer);
        fullmatch = true;
    }
    return fullmatch;
}

/**
 * Try to parse an integer
 * Updates lexer state in case of a match
 * @private
 * @param[in]  lexer The lexer used to extract the integer
 * @returns          Whether an integer can be extracted
 */
static bool Lex_TryParseInteger(lexer_t * lexer) {
    bool fullmatch = false;
    char * char_ptr = lexer->current_char;
    
    if ((*char_ptr == '-' && Lex_InBufferBounds(lexer, char_ptr + 1)
                && *(char_ptr + 1) >= '0' && *(char_ptr + 1) <= '9')
            || (*char_ptr >= '0' && *char_ptr <= '9')) {
        while (++char_ptr && Lex_InBufferBounds(lexer, char_ptr)
                && *char_ptr >= '0' && *char_ptr <= '9') Lex_IncrementCurrentChar(lexer);
        fullmatch = true;
    }
    return fullmatch;
}

/**
 * Try to parse a compound token (STRING, IDENT, COMMENT, INT, DOUBLE)
 * @private
 * @param[in] lexer The lexer used to extract the token
 * @retval A pointer to an allocated token if one is exctracted
 * @retval NULL is none can be found
 */
static token_t * Lex_ParseCompoundToken(lexer_t * lexer) {
    token_t * token = NULL;
    token_type_t token_type = TOKTYPE_NOTTOKEN;
    span_t token_span = {lexer->pos, lexer->pos};

    if (lexer->status != LEX_EOF && lexer->status != LEX_ERROR) {
        if (Lex_TryParseIdentifier(lexer)) {
            token_type = TOKTYPE_IDENT;
        } else if (Lex_TryParseComment(lexer)) {
            token_type = TOKTYPE_COMMENT;
        } else if (Lex_TryParseInteger(lexer)) {
            token_type = TOKTYPE_INT;
            // might also be a double
            if (Lex_NextCharIs(lexer, '.') && Lex_InBufferBounds(lexer, lexer->current_char + 2)
                    && *(lexer->current_char + 2) >= '0' && *(lexer->current_char + 2) <= '9') {
                token_type = TOKTYPE_DOUBLE;
                Lex_IncrementCurrentChar(lexer);
                Lex_IncrementCurrentChar(lexer);
                Lex_TryParseInteger(lexer);
                if (Lex_NextCharIs(lexer, 'e') && Lex_InBufferBounds(lexer, lexer->current_char + 2)
                        && *(lexer->current_char + 2) >= '0' && *(lexer->current_char + 2) <= '9') {
                    Lex_IncrementCurrentChar(lexer);
                    Lex_IncrementCurrentChar(lexer);
                    Lex_TryParseInteger(lexer);
                }
            }
        } else if (Lex_TryParseString(lexer)) {
            token_type = TOKTYPE_STRING;
        }

        if (token_type != TOKTYPE_NOTTOKEN) {
            token_span.end = lexer->pos;
            token = Token_New(token_type, token_span);
            Lex_IncrementCurrentChar(lexer);
        }
    }
    return token;
}

/**
 * Try to parse the next token in the buffer
 * @private
 * @param[in]  lexer The lexer used to extract the token
 * @retval A pointer to the newly extracted token
 * @retval NULL if an error occured
 */
static token_t * Lex_ParseNextToken(lexer_t * lexer) {
    token_t * token;

    token = Lex_ParseCompoundToken(lexer);
    if (!token) {
        token = Lex_ParseSimpleToken(lexer);
    }
    return token;
}

/**
 * Try to extract the next token in the buffer
 * @param[in]  lexer       The lexer used to extract the token
 * @param      preserve_ws When set to true tokens categorized
 *                         as whitespaces will not be ignored
 * @retval A pointer to the newly extracted token
 * @retval NULL if an error occured
 *         Call Lex_GetStatus() for more info
 */
token_t * Lex_NextToken(lexer_t * lexer, bool preserve_ws) {
    token_t * token;

    while(true) {
        token = Lex_ParseNextToken(lexer);
        if (token && !preserve_ws && Token_IsWhitespace(token)) {
            Token_Free(token);
        } else {
            break;
        }
    };
    
    return token;
}

/**
 * Gets the current status of the lexer
 * @returns The current status of the lexer
 */
lexer_status_t Lex_GetStatus(lexer_t * lexer) {
    lexer_status_t status = LEX_ERROR;
    
    if (lexer) {
        status = lexer->status;
    } else {
        Err_Throw(Err_New("NULL pointer to lexer"));
    }
    return status;
}
