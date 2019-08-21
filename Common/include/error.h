/**
 * @file error.h
 * Errors implementation
 */
#pragma once
#include <stdbool.h>
#include "location.h"

typedef struct {
    /**
     * Description of the error
     */
    char * message;
    bool with_location;
    loc_t location;
} error_t;

/**
 * Prints an error
 * @param[in] error The error to print
 */
void Err_Print(error_t * error);

/**
 * Prints an error and terminates the program
 * @param[in] error The error to print
 */
void Err_Throw(error_t * error);

/**
 * Sets the current reported error
 * If current error is not NULL it will be freed
 * @param[in] error The error to set
 */
void Err_SetError(error_t * error);

/**
 * Returns the current reported error
 * @returns The error
 */
error_t * Err_GetError(void);

/**
 * Allocates a new error
 * @param[in] massage The message to associate to the error
 *                    (a copy will be performed)
 * @returns           The newly allocated error
 */
error_t * Err_New(char * message);

/**
 * Allocates a new error
 * @param[in] massage  The message to associate to the error
 *                     (a copy will be performed)
 * @param     location The location to be linked with the error
 *                     (a copy of the filename will be performed)
 * @returns            The newly allocated error
 */
error_t * Err_NewWithLocation(char * message, loc_t location);

/**
 * Frees a previously allocated error
 * @param[in] error The error to free
 */
void Err_Free(error_t * error);
