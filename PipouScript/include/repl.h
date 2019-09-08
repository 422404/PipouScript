/**
 * @file repl.h
 * REPL utilities
 */
#pragma once

#include <string.h>
#include <sys/types.h>

typedef enum {
    REPL_CMD_NONE,
    REPL_CMD_MULTILINE,
} repl_cmd_type_t;

/**
 * Indicates whether a command has been typed
 * and which command it is
 */
repl_cmd_type_t REPL_IsCommand(char * line);

/**
 * Reads characters until EOF
 * @param[out] read Number of chars read
 * @returns         An allocated buffer containing the read chars
 */
char * REPL_ReadMultiLine(ssize_t * read);
