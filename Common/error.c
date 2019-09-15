/**
 * @file error.c
 * Errors implementation
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Common/include/error.h"

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

/**
 * Retrives the source code text on the line where an error occured
 * @param  loc   The location of the error
 * @param buffer The buffer that contains the source code
 * @returns      An allocated string that contains the source code text
 */
char * Err_GetLineString(loc_t loc, char * buffer) {
    size_t start_index, end_index, line;
    size_t buffer_length = strlen(buffer);
    char * str;
    
    // skip the n - 1 first lines
    for (start_index = 0, line = 1; start_index < buffer_length 
            && line < loc.line; start_index++) {
        if (buffer[start_index] == '\n'
                || buffer[start_index] == '\r') {
            line++;
            // if a CR is found we may have a LF following
            // skip the next char if it's the case
            if (buffer[start_index] == '\r'
                    && start_index + 1 < buffer_length
                    && buffer[start_index + 1] == '\n') {
                start_index++;
            }
        }
    }

    // find the end of the line
    for (end_index = start_index; end_index < buffer_length
            && buffer[end_index] != '\n' && buffer[end_index] != '\r';
            end_index++);

    // extract the line
    str = (char *)calloc(1, end_index - start_index + 1);
    if (str) {
        memcpy(str, &buffer[start_index], end_index - start_index);
    }
    return str;
}
