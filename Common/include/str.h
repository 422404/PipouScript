/**
 * @file str.h
 * String implementation
 */
#pragma once

typedef struct {
    char * c_str;
} string;

/**
 * Creates a new string from a C string
 * @param[in] c_str The string that will be copied to construct this one
 *                  (The caller can free it after)
 * @returns The newly allocated string
 */
string * Str_New(char * c_str);

/**
 * Free an allocated string
 * @param[in] str The string to free
 */
void Str_Free(string * str);

/**
 * Appends a string to another one
 * @param[in] str  The string which will receive the appended characters
 * @param[in] str2 The string from which the caracters will be taken
 */ 
void Str_Append(string * str, string * str2);
