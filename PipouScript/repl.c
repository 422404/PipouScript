/**
 * @file repl.c
 * REPL utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include "Common/include/error.h"
#include "repl.h"

/**
 * Indicates whether a command has been typed
 * and which command it is
 */
repl_cmd_type_t REPL_IsCommand(char * line) {
    /// @todo Allow whitespaces before and after commands
    if (strcmp(line, ":ml\n") == 0) return REPL_CMD_MULTILINE;
    return REPL_CMD_NONE;
}

/**
 * Reads characters until EOF
 * @param[out] read Number of chars read
 * @returns         An allocated buffer containing the possibly read chars
 *                  (must always be freed)
 */
char * REPL_ReadMultiLine(ssize_t * read) {
    ssize_t buffer_length = 128;
    char * buffer = malloc(buffer_length);
    char c;

    if (!buffer) Err_Throw(Err_New("[REPL] Multi-line buffer cannot be allocated"));

    *read = 0;
    while ((c = getchar()) != EOF) {
        if (*read == buffer_length - 2) {
            buffer = realloc(buffer, buffer_length * 2);
        }
        buffer[*read] = c;
        (*read)++;
    }
    if (*read) {
        buffer[*read] = '\0';
        (*read)++;
    }
    clearerr(stdin); // Clear persistent EOF flag in stdin 
                     // and enable new attempts to read from it
    return buffer;
}
