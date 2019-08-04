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
#include "Parser/include/parser.h"
#include "ast.h"

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
 * Prints the token stream from a buffer that contains code
 * @param[in] buffer   The buffer to extract the tokens from
 * @param[in] filename The name of the file that contains the code
 */
static void Eval_PrintTokens(char * buffer, char * filename) {
    lexer_t * lexer;
    token_t * token;

    lexer = Lex_New(buffer, strlen(buffer), filename);
    printf("\n");
    while (Lex_GetStatus(lexer) == LEX_OK) {
        token = Lex_NextToken(lexer, false);
        if (token) {
            char * token_string = Token_ToString(token);
            printf("%s\n", token_string);
            free(token_string);
        }
        if (Lex_GetStatus(lexer) == LEX_ERROR) {
            Err_Print(Err_GetError());
            Err_SetError(NULL);
        }
    }
    Lex_Free(lexer);
}

/**
 * Prints the AST from a buffer that contains code
 * @param[in] buffer   The buffer to extract the AST from
 * @param[in] filename The name of the file that contains the code
 */
static void Eval_PrintAST(char * buffer, char * filename) {
    parser_t * parser;
    ast_node_t * ast_root;

    parser = Parser_New(buffer, strlen(buffer), filename, true);
    ast_root = Parser_CreateAST(parser);
    if (ast_root) {
        char * ast_string = ASTNode_ToString(ast_root);
        printf("\n%s\n", ast_string);
        free(ast_string);
    } else {
        Err_Print(Err_GetError());
        Err_SetError(NULL);
    }
    Parser_Free(parser);
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
            /// @todo only when some flag is set
            Eval_PrintTokens(line, NULL);
            Eval_PrintAST(line, NULL);
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
