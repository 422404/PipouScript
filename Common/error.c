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
    if (error->with_location) {
        printf("[Error]: %s(%ld:%ld)\n%s\n", 
                error->location.filename ? error->location.filename : "<buffer>",
                error->location.line, error->location.col, error->message);
    } else {
        printf("[Error]: %s\n", error->message);
    }
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

    error = (error_t *)malloc(sizeof(error_t));
    error->message = strdup(message);
    error->with_location = false;
    return error;
}

/**
 * Allocates a new error
 * @param[in] massage  The message to associate to the error
 *                     (a copy will be performed)
 * @param     location The location to be linked with the error
 *                     (a copy of the filename will be performed)
 * @returns            The newly allocated error
 */
error_t * Err_NewWithLocation(char * message, loc_t location) {
    error_t * error;
    loc_t loc = {
        location.line,
        location.col,
        location.filename ? strdup(location.filename) : NULL
    };
    error = (error_t *)malloc(sizeof(error_t));
    error->message = strdup(message);
    error->with_location = true;
    error->location = loc;
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
