/**
 * @file lexer.c
 * Input lexical analysis
 */
#include <stdlib.h>
#include <stdbool.h>
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
 * @param[in] buffer The buffer where tokens will be extracted
 * @param     length Length of the input buffer
 * @returns          A pointer to the newly allocated lexer
 */
lexer_t * Lex_New(char * buffer, size_t length) {
    lexer_t * lexer = NULL;

    if (buffer == NULL) Err_Throw(Err_New("NULL pointer to lexer chars buffer"));

    lexer = (lexer_t *)malloc(sizeof(lexer_t));
    if (lexer) {
        pos_t pos = {1, 1};
        lexer->buffer = buffer;
        lexer->buffer_length = length;
        lexer->current_char = buffer;
        lexer->token_type = TOKTYPE_NOTTOKEN;
        lexer->pos = pos;
        lexer->status = length != 0 ? LEX_OK : LEX_EOF;
        lexer->error = NULL;
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
    if (lexer) {
        if (lexer->error) free(lexer->error);
        free(lexer);
    }
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
 * Try to parse a simple token
 * @private
 * @param[in] lexer The lexer used to extract the token
 * @retval A pointer to an allocated token if one is exctracted
 * @retval NULL is none can be found
 */
static token_t * Lex_ParseSimpleToken(lexer_t * lexer) {
    token_t * token = NULL;
    token_type_t token_type = TOKTYPE_NOTTOKEN;
    pos_t token_start, token_end;
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
        }

        if (token_type == TOKTYPE_EQUAL
                && Lex_NextCharIs(lexer, TOK_EQUAL)) {
            token_type = TOKTYPE_EQEQUAL;
            Lex_IncrementCurrentChar(lexer);
            token_end.col++;

        } else if (c == '\n' || c == '\r') {
            token_type = TOKTYPE_NEWLINE;
            if (c == '\r' && Lex_NextCharIs(lexer, '\n')) {
                Lex_IncrementCurrentChar(lexer);
                token_end.col++;
            }
        }

        if (token_type != TOKTYPE_NOTTOKEN) { // if matched
            Lex_IncrementCurrentChar(lexer);
            token_span.start = token_start;
            token_span.end   = token_end;
            token = Token_New(token_type, token_span);
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
            lexer->error = Err_New("String not terminated");
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
                Lex_TryParseInteger(lexer);
                if (Lex_NextCharIs(lexer, 'e') && Lex_InBufferBounds(lexer, lexer->current_char + 2)
                        && *(lexer->current_char + 2) >= '0' && *(lexer->current_char + 2) <= '9') {
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
