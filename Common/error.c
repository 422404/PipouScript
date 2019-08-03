/**
 * @file error.c
 * Errors implementation
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

/** Not thread-safe */
static error_t * current_error = NULL;

/**
 * Prints an error
 * @param[in] error The error to print
 */
void Err_Print(error_t * error) {
    printf("%s\n", error->message);
}

/**
 * Prints an error and terminates the program
 * @param[in] error The error to print
 */
void Err_Throw(error_t * error) {
    Err_Print(error);
    Err_Free(error);
    exit(-1);
}

/**
 * Sets the current reported error
 * If current error is not NULL it will be freed
 * @param[in] error The error to set
 */
void Err_SetError(error_t * error) {
    if (current_error) Err_Free(current_error);
    current_error = error;
}

/**
 * Returns the current reported error
 * @returns The error
 */
error_t * Err_GetError(void) {
    return current_error;
}

/**
 * Allocates a new error
 * @param[in] massage The message to associate to the error
 *                    (a copy will be performed)
 * @returns           The newly allocated error
 */
error_t * Err_New(char * message) {
    error_t * error;
    char * msg_copy;
    size_t msg_buf_length;

    error = (error_t *)malloc(sizeof(error_t));
    msg_buf_length = strlen(message) + 1;
    msg_copy = (char *)malloc(msg_buf_length);
    memcpy(msg_copy, message, msg_buf_length);
    error->message = msg_copy;
    return error;
}

/**
 * Frees a previously allocated error
 * @param[in] error The error to free
 */
void Err_Free(error_t * error) {
    free(error->message);
    free(error);
}
