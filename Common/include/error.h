/**
 * @file error.h
 * Errors implementation
 */
#pragma once

typedef struct {
    /**
     * Description of the error
     */
    char * message;
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
 * Allocates a new error
 * @param[in] massage The message to associate to the error
 * @returns           The newly allocated error
 */
error_t * Err_New(char * message);

/**
 * Frees a previously allocated error
 * @param[in] error The error to free
 */
void Err_Free(error_t * error);
