// clang-format off
/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project4
 * kvs_lru.c
 * key-value LRU cache policy
 *
 * Notes:
 * - When file is empty or '\0', GET still adds to cache.
 * - Using memmove instead of regular shift to safely handle overlap when shifting array.
 *
 * Usage:
 * See comments near functions.
 *
 * Citations:
 * - Dongjing tutor
 * - Rohan tutor
 * - https://git.ucsc.edu/jma363/cse101-winter2022/-/blob/main/pa4/List.c
 *
 *********************************************************************************/
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_lru.h"
#include <stdbool.h> // newly added to use bool data type
#include <stdlib.h>
#include <string.h>

// structs --------------------------------------------------------------------

struct kvs_lru {
  kvs_base_t* kvs_base; // base key-value store
  char** keys; // array of keys
  char** values; // added a values field
  bool* dirty; // changed int* to bool*
  int capacity; // maximum capacity of LRU cache
  int size; // current size of cache
  int replacement_count; // added a replacement_count field
};

// Constructors-Destructors ---------------------------------------------------

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t)); // allocate memory base of key-value store
  kvs_lru->kvs_base = kvs; // set base key-value store
  kvs_lru->capacity = capacity; // set capacity of cache
  kvs_lru->size = 0; // initialize size of 0
  kvs_lru->keys = calloc(capacity, sizeof(char*)); // allocate memory for keys
  kvs_lru->values = calloc(capacity, sizeof(char*)); // allocate mem for values
  kvs_lru->dirty = calloc(capacity, sizeof(bool)); // changed int to bool
  kvs_lru->replacement_count = 0; // initialize replacement_count to 0
  return kvs_lru; // return newly created LRU cache
}

void kvs_lru_free(kvs_lru_t** ptr) { // memory unsafe function to free memory from LRU cache
  for (int i = 0; i < (*ptr)->size; ++i) { // iterate over each element in cache
    free((*ptr)->keys[i]); // free each key
    free((*ptr)->values[i]); // free each val
  }
  free((*ptr)->keys); // free mem allocated for keys arr
  free((*ptr)->values); // free mem allocated for vals arr
  free((*ptr)->dirty); // free mem allocated for dirty arr
  free(*ptr); // free memory allocated for kvs_lru
  *ptr = NULL; // drop pointer
}

// Manipulation and access procedures -----------------------------------------

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) { // fn to add key-value pair to LRU cache
  for (int i = 0; i < kvs_lru->size; ++i) { // iterate thorugh each value in cache
    if (strcmp(kvs_lru->keys[i], key) == 0) { // if key is found in cache, update value
      free(kvs_lru->values[i]); // free old value
      kvs_lru->values[i] = strdup(value); // set new value
      kvs_lru->dirty[i] = true; // mark as dirty

      char* temp_key = kvs_lru->keys[i]; // backup current key
      char* temp_value = kvs_lru->values[i]; // backup value
      bool temp_dirty = kvs_lru->dirty[i]; // backup dirty flag
      // regular method used instead of memmove because we have small controlled shift with no risk of source and destination memory areas overlapping
      for (int j = i; j < kvs_lru->size - 1; ++j) { // move all keys, values, dirty flags one position towards head of cache
        kvs_lru->keys[j] = kvs_lru->keys[j+1];
        kvs_lru->values[j] = kvs_lru->values[j+1];
        kvs_lru->dirty[j] = kvs_lru->dirty[j+1];
      }
      kvs_lru->keys[kvs_lru->size - 1] = temp_key; // move updated key, values, dirty flags to end of cache (most recently used position)
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;

      return 0; // return success
    }
  } // proceeding below if key not in cache ------

  if (kvs_lru->size == kvs_lru->capacity) { // if the key is not found in the cache and cache is full
    if (kvs_lru->dirty[0]) { // if the least recently used (LRU) entry is dirty, update in disk
      int rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]);
      if (rc != 0) return rc; // if updating disk store fails indicated by a return code other than success, return return-code
      kvs_lru->replacement_count++; // increment replacement_count here because a dirty pair was replaced
    }

    free(kvs_lru->keys[0]); // free least recently used (LRU) key
    free(kvs_lru->values[0]); // free least recently used (LRU) val
    
    // using the memmove function to move all keys, values, and dirty flags one position towards head of the cache
    // memmove used here instead of manually shifting each entry because memmove safely handles overlap between source and destination memory areas
    memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(bool));
  } else {
    ++kvs_lru->size; // else if cache not full, increase its size
  }

  kvs_lru->keys[kvs_lru->size - 1] = strdup(key); // add new key to end of cache (most recently used position)
  kvs_lru->values[kvs_lru->size - 1] = strdup(value); // add new val to end of cache (most recently used position)
  kvs_lru->dirty[kvs_lru->size - 1] = true; // mark new pair as modified

  return 0; // return success
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) { // fn to get value associated with a key from LRU cache
  for (int i = 0; i < kvs_lru->size; ++i) { // iterate thorugh all elements in cache
    if (strcmp(kvs_lru->keys[i], key) == 0) { // if key is present in cache
      strcpy(value, kvs_lru->values[i]); // copy value to provided pointer buffer

      char* temp_key = kvs_lru->keys[i]; // backup curr key
      char* temp_value = kvs_lru->values[i]; // backup val
      bool temp_dirty = kvs_lru->dirty[i]; // backup dirty status
      // regular method used instead of memmove because we have small controlled shift with no risk of source and destination memory areas overlapping
      for (int j = i; j < kvs_lru->size - 1; ++j) { // move all keys, values, and dirty flag one position towards head of cache
        kvs_lru->keys[j] = kvs_lru->keys[j+1];
        kvs_lru->values[j] = kvs_lru->values[j+1];
        kvs_lru->dirty[j] = kvs_lru->dirty[j+1];
      }
      kvs_lru->keys[kvs_lru->size - 1] = temp_key; // move accessed key-valued pair to end of cache (most recently used position)
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;

      return 0; // return success
    }
  } // procced below if key is not found in the cache, get it from disk

  int rc = kvs_base_get(kvs_lru->kvs_base, key, value); // probe return code

  if (rc == 0) { // if key not in disk, add key-value pair to cache
    if (kvs_lru->size == kvs_lru->capacity) { // if cache is full
      if (kvs_lru->dirty[0]) { // if key dirty
        rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]); // probe return code
        if (rc != 0) return rc; // if return code is not success, then return return-code
        kvs_lru->replacement_count++; // increment replacement_count here because a dirty pair was replaced
      }

      free(kvs_lru->keys[0]); // free least recently used (LRU) entry
      free(kvs_lru->values[0]); // free least recently used (LRU) entry

      // using the memmove function to move all keys, values, and dirty flags one position towards head of the cache
      // memmove used here instead of manually shifting each entry because memmove safely handles overlap between source and destination memory areas
      memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(bool));
    } else {
      ++kvs_lru->size; // if cache is not full, increase its size
    }

    kvs_lru->keys[kvs_lru->size - 1] = strdup(key); // add new key at end of cache (most recently used position)
    kvs_lru->values[kvs_lru->size - 1] = strdup(value); // add new val at end of cache (most recently used position)
    kvs_lru->dirty[kvs_lru->size - 1] = false; // mark new pair as clean/false entry becuase it was just from the disk
  }

  return rc; // return return-code of kvs_base_get
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  for (int i = 0; i < kvs_lru->size; ++i) { // iterate through all elements in cache
    if (kvs_lru->dirty[i]) { // if entry is dirty
      int rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[i], kvs_lru->values[i]); // probe for return code
      if (rc != 0) return rc; // if write to disk fail, return return-code
      kvs_lru->dirty[i] = false; // if dirty clean after write
    }
    free(kvs_lru->keys[i]); // free key
    free(kvs_lru->values[i]); // free val
  }
  kvs_lru->size = 0; // reset size to 0 to clear it
  kvs_lru->replacement_count = 0; // reset replacement_count after flushing

  return 0; // return success
}
