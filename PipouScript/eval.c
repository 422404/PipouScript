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
#include "str.h"
#include "repl.h"

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
    bool first_token_printed = false;

    lexer = Lex_New(buffer, strlen(buffer), filename);
    while (Lex_GetStatus(lexer) == LEX_OK) {
        token = Lex_NextToken(lexer, false, true);
        if (token) {
            char * token_string = Token_ToString(token);
            if (!first_token_printed) {
                printf("\n");
                first_token_printed = true;
            }
            printf("%s\n", token_string);
            free(token_string);
            Token_Free(token);
        }
        if (Lex_GetStatus(lexer) == LEX_ERROR) {
            error_t * error = Lex_GetError(lexer);
            Err_Print(error);
            if (error->with_location) {
                char * line = Err_GetLineString(error->location, buffer);
                printf("%s\n", line);
                free(line);
            }
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
    ast_root = Parser_CreateAST(parser, false);
    if (ast_root) {
        string * ast_string = ASTNode_ToString(ast_root);
        printf("\n%s\n", ast_string->c_str);
        Str_Free(ast_string);
        ASTNode_Free(ast_root);
    } else if (Parser_GetStatus(parser) == PARSER_ERROR) {
        error_t * error = Parser_GetError(parser);
        Err_Print(error);
        if (error->with_location) {
            char * line = Err_GetLineString(error->location, buffer);
            printf("%s\n", line);
            free(line);
        }
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
    bool must_loop = true;
    bool multi_line = false;

    printf("Welcome to PipouScript shell v%s\n", PRINT_VERSION);
    while(must_loop) {
        if (!multi_line) {
            printf(":> ");
            line_length = Eval_ReadLine(&line, &tmp);
        } else {
            line = REPL_ReadMultiLine(&line_length);
            multi_line = false;
        }
        if (line_length == -1) {
            if (feof(stdin)) must_loop = false;
            else {
                perror("[REPL] ");
                return -1;
            }
        } else if (line_length > 1) {
            repl_cmd_type_t cmd = REPL_IsCommand(line);
            if (cmd == REPL_CMD_NONE) {
                /// @todo only when some flag is set
                Eval_PrintTokens(line, NULL);
                Eval_PrintAST(line, NULL);
            } if (cmd == REPL_CMD_MULTILINE) {
                multi_line = true;
            }
        }
        free(line);
    }

    return 0;
}
