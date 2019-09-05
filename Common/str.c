/**
 * @file str.c
 * String implementation
 */
#include <string.h>
#include <stdlib.h>
#include "str.h"
#include "error.h"

/**
 * Creates a new string from a C string
 * @param[in] c_str The C string that will be copied to construct this one
 *                  (The caller can free it after)
 * @returns The newly allocated string
 */
string * Str_New(char * c_str) {
    string * str;

    if (!c_str) Err_Throw(Err_New("NULL pointer to c_str"));
    str = (string *)malloc(sizeof(string));
    if (str) {
        str->c_str = strdup(c_str);
        if (!str->c_str) Err_Throw(Err_New("Cannot copy c_str"));
    }
    return str;
}

/**
 * Free an allocated string
 * @param[in] str The string to free
 */
void Str_Free(string * str) {
    if (!str) Err_Throw(Err_New("NULL pointer to str"));
    free(str->c_str);
    free(str);
}

/**
 * Appends a string to another one
 * @param[in] str  The string which will receive the appended characters
 * @param[in] str2 The string from which the caracters will be taken
 */ 
void Str_Append(string * str, string * str2) {
    size_t length;
    char * newbuf;

    if (!str) Err_Throw(Err_New("NULL pointer to str"));
    if (!str2) Err_Throw(Err_New("NULL pointer to str2"));

    length = strlen(str->c_str) + strlen(str2->c_str) + 1;
    newbuf = realloc(str->c_str, length);
    if (newbuf) {
        str->c_str = newbuf;
        strcat(newbuf, str2->c_str);
    } else {
        Err_Throw(Err_New("Cannot resize str"));
    }
}
