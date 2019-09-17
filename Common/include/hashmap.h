/**
 * @file hashmap.h
 * Hash map implementation
 */
#pragma once
#include <sys/types.h>
#include <stdbool.h>

typedef struct hashmap_entry_s hashmap_entry_t;

typedef struct hashmap_entry_s {
    char * key;
    void * value;
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
 * @param[out] value   The value stored
 *                     Please note that if using values other than pointers
 *                     the pointer provided must point to memory aligned
 *                     to the size of a pointer
 * @returns            Whether the lookup was successful
 */
bool HashMap_Get(hashmap_t * hashmap, char * key, void ** value);

/**
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 * @param[in] value   The value to store
 *                    Please note that anything other than a pointer
 *                    can be stored but take great care when using
 *                    HashMap_Get()
 */
void HashMap_Set(hashmap_t * hashmap, char * key, void * value);

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
