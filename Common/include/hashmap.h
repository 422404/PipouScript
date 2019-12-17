/**
 * @file hashmap.h
 * Hash map implementation using NaN-boxing
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "nanbox.h"
#include "vector.h"

typedef struct hashmap_entry_s hashmap_entry_t;

typedef struct hashmap_entry_s {
    char * key;
    nanbox_t value;
    /** Next entry in case of key hash collision */
    hashmap_entry_t * next;
} hashmap_entry_t;

typedef struct {
    /** Quick access slots */
    hashmap_entry_t ** entries;
    /** Number of elements if hashmap */
    size_t count;
    /** Number of elements in entries[] */
    size_t entries_count;
} hashmap_t;

/**
 * Allocates an hashmap with a default capacity of 16
 * @returns The allocated hashmap
 */
hashmap_t * HashMap_New();

/**
 * Allocates a hashmap with a given capacity
 * @param capacity Initial capacity of the hashmap
 *                 Will be changed when load factor
 *                 reach the 0.75 threshold
 * @returns        The allocated hashmap
 */
hashmap_t * HashMap_NewWithCapacity(size_t capacity);

/**
 * Free an allocated hashmap
 * @param[in] hashmap The hashmap to be freed
 */
void HashMap_Free(hashmap_t * hashmap);

/**
 * @param[in]  hashmap The hashmap to work on
 * @param[in]  key     The key of the (key, val) pair
 * @param[out] value   The stored value
 */
bool HashMap_Get(hashmap_t * hashmap, char * key, nanbox_t * value);

/**
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 * @param     value   The value to store
 */
void HashMap_Set(hashmap_t * hashmap, char * key, nanbox_t value);

/**
 * Removes a (key, value) pair from a hashmap
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 */
void HashMap_Remove(hashmap_t * hashmap, char * key);

/**
 * Checks if a (key, val) pair is present in a hashmap
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 * @returns           Whether the (key, val) pair is present
 */
bool HashMap_Contains(hashmap_t * hashmap, char * key);

/**
 * Returns a vector with all the entries of a hashmap
 * @param[in] hashmap The hashmap to work on
 * @returns           The entries list (vector_t<nanbox_t>)
 */
vector_t * HashMap_GetValues(hashmap_t * hashmap);

/**
 * Returns a vector with all the keys of a hashmap
 * @param[in] hashmap The hashmap to work on
 * @returns           The keys list (vector_t<nanbox_t>)
 *                    Keys must be freed
 */
vector_t * HashMap_GetKeys(hashmap_t * hashmap);
