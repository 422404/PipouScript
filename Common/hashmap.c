/**
 * @file hashmap.c
 * Hash map implementation using NaN-boxing
 */
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"
#include "Common/include/error.h"
#include "nanbox.h"

#define DEFAULT_INITIAL_CAPACITY 16
#define LOAD_FACTOR 0.75

/********************** HashMapEntry Functions ******************************/

static hashmap_entry_t * HashMapEntry_New(char * key, nanbox_t value) {
    hashmap_entry_t * entry = (hashmap_entry_t *)malloc(sizeof(hashmap_entry_t));
    if (entry) {
        entry->key = key;
        entry->value = value;
        entry->next = NULL;
    } else {
        Err_Throw(Err_New("Cannot allocate HashMap entry"));
    }
    return entry;
}

static void HashMapEntry_Free(hashmap_entry_t * entry) {
    if (!entry) Err_Throw(Err_New("NULL pointer to HashMap entry"));
    free(entry);
}

/********************** HashMap Functions ******************************/

static size_t HashMap_HashString(char * str) {
    size_t hash = 5381, c;

    while ((c = *str++)) hash = ((hash << 5) + hash) ^ c;

    return hash;
}

static bool HashMap_OptimalSize(double count, double entries_count) {
    return count / entries_count <= LOAD_FACTOR;
}

static void HashMap_Grow(hashmap_t * hashmap) {
    size_t new_entries_count, old_entries_count;
    hashmap_entry_t ** new_entries, ** old_entries;
    hashmap_entry_t * cur, * next;
    
    old_entries_count = hashmap->entries_count;
    old_entries = hashmap->entries;
    new_entries_count = old_entries_count * 2;
    new_entries = (hashmap_entry_t **)calloc(
            new_entries_count, sizeof(hashmap_entry_t *));
    if (!new_entries) Err_Throw(Err_New("Cannot grow HashMap"));

    hashmap->entries = new_entries;
    hashmap->entries_count = new_entries_count;
    hashmap->count = 0;

    for (size_t i = 0; i < old_entries_count; i++) {
        if (old_entries[i]) {
            next = old_entries[i];
            while (next) {
                cur = next;
                next = cur->next;
                HashMap_Set(hashmap, cur->key, cur->value);
                HashMapEntry_Free(cur);
            }
        }
    }
    free(old_entries);
}

hashmap_t * HashMap_NewWithCapacity(size_t capacity) {
        hashmap_t * hashmap;

    hashmap = (hashmap_t *)malloc(sizeof(hashmap_t));
    if (hashmap) {
        hashmap->count = 0;
        hashmap->entries_count = capacity;
        hashmap->entries = (hashmap_entry_t **)calloc(
                capacity, sizeof(hashmap_entry_t *));
    }
    if (!hashmap || !hashmap->entries) {
        if (hashmap) free(hashmap);
        Err_Throw(Err_New("Cannot allocate HashMap"));
    }
    return hashmap;
}

hashmap_t * HashMap_New(void) {
    return HashMap_NewWithCapacity(DEFAULT_INITIAL_CAPACITY);
}

void HashMap_Free(hashmap_t * hashmap) {
    hashmap_entry_t * cur, * next;

    if (!hashmap) Err_Throw(Err_New("NULL pointer to HashMap"));
    for (size_t i = 0; i < hashmap->entries_count; i++) {
        if (hashmap->entries[i]) {
            next = hashmap->entries[i];
            while (next) {
                cur = next;
                next = cur->next;
                HashMapEntry_Free(cur);
            }
        }
    }
    free(hashmap->entries);
    free(hashmap);
}

/**
 * @param[in]  hashmap The hashmap to work on
 * @param[in]  key     The key of the (key, val) pair
 * @param[out] value   The stored value
 */
bool HashMap_Get(hashmap_t * hashmap, char * key, nanbox_t * value) {
    bool found = false;
    size_t index = HashMap_HashString(key) % hashmap->entries_count;
    hashmap_entry_t * entry = hashmap->entries[index];
    
    while (entry && !found) {
        if (strcmp(key, entry->key) == 0) {
            found = true;
            *value = entry->value;
        } else {
            entry = entry->next;
        }
    }
    return found;
}

/**
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 * @param     value   The value to store
 */
void HashMap_Set(hashmap_t * hashmap, char * key, nanbox_t value) {
    bool got_entries = false, found = false;
    hashmap_entry_t * cur, * entry;

    if (!HashMap_OptimalSize(hashmap->count + 1, hashmap->entries_count)) {
        HashMap_Grow(hashmap);
    }
    size_t index = HashMap_HashString(key) % hashmap->entries_count;
    cur = hashmap->entries[index];
    got_entries = !!cur;
    
    if (got_entries) {
        while (cur && !found) {
            if (strcmp(key, cur->key) == 0) {
                found = true;
                cur->value = value;
            } else {
                cur = cur->next;
            }
        }
    }
    if (!got_entries || !found) {
        entry = HashMapEntry_New(key, value);
        if (got_entries) {
            entry->next = hashmap->entries[index];
        }
        hashmap->entries[index] = entry;
        hashmap->count++;
    }
}

void HashMap_Remove(hashmap_t * hashmap, char * key) {
    bool found = false;
    size_t index = HashMap_HashString(key) % hashmap->entries_count;
    hashmap_entry_t * prev, * entry;
    prev = entry = hashmap->entries[index];
    
    while (entry && !found) {
        if (strcmp(key, entry->key) == 0) {
            found = true;
            if (prev == entry) {
                hashmap->entries[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            HashMapEntry_Free(entry);
            hashmap->count--;
        } else {
            prev = entry;
            entry = entry->next;
        }
    }
}

/**
 * Checks if a (key, val) pair is present in a hashmap
 * @param[in] hashmap The hashmap to work on
 * @param[in] key     The key of the (key, val) pair
 * @returns           Whether the (key, val) pair is present
 */
bool HashMap_Contains(hashmap_t * hashmap, char * key) {
    nanbox_t tmp;
    return HashMap_Get(hashmap, key, &tmp);
}

/**
 * Returns a vector with all the entries of a hashmap
 * @param[in] hashmap The hashmap to work on
 * @returns           The entries list (vector_t<nanbox_t>)
 */
vector_t * HashMap_GetValues(hashmap_t * hashmap) {
    hashmap_entry_t  * next;
    vector_t * vec = Vec_New();

    for (size_t i = 0; i < hashmap->entries_count; i++) {
        if (hashmap->entries[i]) {
            next = hashmap->entries[i];
            while (next) {
                Vec_Append(vec, next->value);
                next = next->next;
            }
        }
    }
    return vec;
}

/**
 * Returns a vector with all the keys of a hashmap
 * @param[in] hashmap The hashmap to work on
 * @returns           The keys list (vector_t<nanbox_t>)
 */
vector_t * HashMap_GetKeys(hashmap_t * hashmap) {
    hashmap_entry_t  * next;
    vector_t * vec = Vec_New();

    for (size_t i = 0; i < hashmap->entries_count; i++) {
        if (hashmap->entries[i]) {
            next = hashmap->entries[i];
            while (next) {
                Vec_Append(vec, nanbox_from_pointer(strdup(next->key)));
                next = next->next;
            }
        }
    }
    return vec;
}
