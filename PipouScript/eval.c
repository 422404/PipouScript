/**
 * @file eval.c
 * Evaluation of user input in interactive mode
 */
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>

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
ssize_t ReadLine(char ** line_ptr, size_t * len) {
    *line_ptr = NULL;
    *len = 0;
    return getline(line_ptr, len, stdin);
}

/**
 * Puts the interpreter in REPL mode and waits for input
 * @retval 0 on termination without error
 * @retval Non-zero on failure
 */
int EvalLoop() {
    char * line;
    size_t tmp = 0;
    ssize_t line_length;

    printf("Welcome to PipouScript shell v%s\n", PRINT_VERSION);
    while(true) {
        printf("] ");
        line_length = ReadLine(&line, &tmp);
        if (line_length > 0) {
            printf("%s", line);
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
