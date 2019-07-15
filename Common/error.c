/**
 * @file error.c
 * Errors implementation
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

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
