/**
 * @file eval.c
 * Evaluation of user input in interactive mode
 */
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tokens.h"
#include "token.h"
#include "lexer.h"

#ifdef BUILD_VERSION
#define PRINT_VERSION BUILD_VERSION
#else
#define PRINT_VERSION "<unspecified>"
#endif

/**
 * Reads one line from the shell
 * @param[out] line_ptr Pointer to the allocated memory storing the line read
 * @param[out] len      Length of the line read
 * @retval String length of the line (don't include '\0')
 * @retval -1 on failure
 */
static ssize_t Eval_ReadLine(char ** line_ptr, size_t * len) {
    *line_ptr = NULL;
    *len = 0;
    return getline(line_ptr, len, stdin);
}

/**
 * @todo doc
 * @todo take filename as param
 */
void Eval_PrintTokens(char * buffer) {
    lexer_t * lexer;
    token_t * token;

    lexer = Lex_New(buffer, strlen(buffer), NULL /** @todo take filename as param */);
    while (Lex_GetStatus(lexer) == LEX_OK) {
        token = Lex_NextToken(lexer, false);
        if (token) {
            printf("Token { %s, (%ld:%ld), (%ld:%ld) }\n", token_type_names[token->type],
                    token->span.start.line, token->span.start.col,
                    token->span.end.line, token->span.end.col);
        }
        if (Lex_GetStatus(lexer) == LEX_ERROR) {
            printf("Lexer error: ");
            if (lexer->error) Err_Print(lexer->error);
        }
    }
    Lex_Free(lexer);
}

/**
 * Puts the interpreter in REPL mode and waits for input
 * @retval 0 on termination without error
 * @retval Non-zero on failure
 */
int Eval_REPL() {
    char * line;
    size_t tmp = 0;
    ssize_t line_length;

    printf("Welcome to PipouScript shell v%s\n", PRINT_VERSION);
    while(true) {
        printf(":> ");
        line_length = Eval_ReadLine(&line, &tmp);
        if (line_length > 0) {
            Eval_PrintTokens(line);
            free(line);
        } else if (line_length == -1) {
            if (feof(stdin)) return 0;
            else {
                perror("Error: ");
                return -1;
            }
        }
    }
}
