/**
 * @file token.c
 * Token implementation
 */
#include "token.h"
#include "tokens.h"

/**
 * @param[in] token A pointer to the token
 * @retval true if the token is a whitespace
 * @retval false if it's not
 */
bool Token_IsWhitespace(token_t * token) {
    return token->type == TOKTYPE_SPACE
            || token->type == TOKTYPE_TAB
            || token->type == TOKTYPE_NEWLINE;
}
