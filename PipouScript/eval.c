#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
#include "eval.h"

#ifdef BUILD_VERSION
#define PRINT_VERSION BUILD_VERSION
#else
#define PRINT_VERSION "<unspecified>"
#endif

ssize_t ReadLine(char ** line_ptr, size_t * len) {
    *line_ptr = NULL;
    *len = 0;
    return getline(line_ptr, len, stdin);
}

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
