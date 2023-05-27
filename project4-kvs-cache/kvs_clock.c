// clang-format off
/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project4
 * kvs_clock.c
 * key-value clock cache policy
 *
 * Notes:
 * - When file is empty or '\0', GET still adds to cache.
 * - When a new entry is added, the reference bit of that entry is set to 1.
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
#include "kvs_clock.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// structs --------------------------------------------------------------------

struct kvs_clock {
  kvs_base_t* kvs_base; // base key-value store
  int capacity; // maximum capacity of COCK cache
  char **keys; // dynamic array of keys
  char **values; // dynamic array of values
  int *ref_bits; // dynamic array of reference bits
  bool *dirty; // newly added dirty array
  int size; // current size of cache
  int cursor; // index position of cursor
};

// Constructors-Destructors ---------------------------------------------------

kvs_clock_t* kvs_clock_new(kvs_base_t* kvs, int capacity) {
  kvs_clock_t* kvs_clock = malloc(sizeof(kvs_clock_t)); // base of key-value store
  kvs_clock->kvs_base = kvs; // set base key-value store
  kvs_clock->capacity = capacity; // set capacity of cache
  kvs_clock->keys = calloc(capacity, sizeof(char*)); // allocate memory for keys arr
  kvs_clock->values = calloc(capacity, sizeof(char*)); // alocate mem for vals arr
  kvs_clock->ref_bits = calloc(capacity, sizeof(int)); // newly initialized reference bits array
  kvs_clock->dirty = calloc(capacity, sizeof(bool)); // newly initialized dirt array
  kvs_clock->size = 0; // initialize size of 0
  kvs_clock->cursor = 0; // set cursor index to beginning of cache
  return kvs_clock; // return newly created cache
}

void kvs_clock_free(kvs_clock_t** ptr) { // memory unsafe function to free memory from COCK cache
  for (int i = 0; i < (*ptr)->size; ++i) { // iterate over each element in cache
    free((*ptr)->keys[i]); // free each key
    free((*ptr)->values[i]); // free each val
  }
  free((*ptr)->keys); // free mem allocated for keys arr
  free((*ptr)->values); // free mem allocated for vals arr
  free((*ptr)->ref_bits); // free mem allocated for reference bits arr
  free((*ptr)->dirty); // newly free dirty arr
  free(*ptr); // free memory allocated for kvs_clock
  *ptr = NULL; // drop pointer
}

// Manipulation and access procedures -----------------------------------------

int kvs_clock_set(kvs_clock_t* kvs_clock, const char* key, const char* value) {
  for (int i = 0; i < kvs_clock->size; ++i) { // iterate through each entry in cache
    if (strcmp(kvs_clock->keys[i], key) == 0) { // if key in cache, update value
      //printf("[i] Setting key '%s' which already exists in the cache at index %d\n", key, i);
      free(kvs_clock->values[i]); // free old value
      kvs_clock->values[i] = strdup(value); // set new value
      kvs_clock->ref_bits[i] = 1; // set ref bit to 1
      kvs_clock->dirty[i] = true; // mark as dirty
      return 0; // return success
    }
  } // proceeding below if key not in cache ------

  if (kvs_clock->size < kvs_clock->capacity) { // if cache not full, add key-value pair to cache
    //printf("[i] Cache is not full. Adding key '%s' at index %d\n", key, kvs_clock->size);
    kvs_clock->keys[kvs_clock->size] = strdup(key); // add key
    kvs_clock->values[kvs_clock->size] = strdup(value); // add value
    kvs_clock->dirty[kvs_clock->size] = true; // mark as dirty
    kvs_clock->size++; // increase cache size
  } else { // if cache is full, replace key-value pair using cock algorithm
    while (kvs_clock->ref_bits[kvs_clock->cursor]) { // find key-value pair with ref bit of 0
      //printf("[i] Cache is full. Replacing key '%s' at index %d\n", key, kvs_clock->cursor);
      kvs_clock->ref_bits[kvs_clock->cursor] = 0; // reset ref bit
      kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity; // move cursor
    }

    if (kvs_clock->dirty[kvs_clock->cursor]) { // if key-value pair is dirty, write back to disk
      kvs_base_set(kvs_clock->kvs_base, kvs_clock->keys[kvs_clock->cursor], kvs_clock->values[kvs_clock->cursor]);
      kvs_clock->dirty[kvs_clock->cursor] = false; // reset dirty flag
    }

    free(kvs_clock->keys[kvs_clock->cursor]); // free old key
    free(kvs_clock->values[kvs_clock->cursor]); // free old value
    kvs_clock->keys[kvs_clock->cursor] = strdup(key); // set new key
    kvs_clock->values[kvs_clock->cursor] = strdup(value); // set new value
    kvs_clock->dirty[kvs_clock->cursor] = true; // mark as dirty
    kvs_clock->ref_bits[kvs_clock->cursor] = 1; // set the reference bit to 1
  }
  
  return 0; // return success without calling kvs_base_set when updating to disk
}

int kvs_clock_get(kvs_clock_t* kvs_clock, const char* key, char* value) {
  for (int i = 0; i < kvs_clock->size; ++i) {
    if (strcmp(kvs_clock->keys[i], key) == 0) { // if key is found in cache
      //printf("[i] Key '%s' found in cache at index %d\n", key, i);
      strcpy(value, kvs_clock->values[i]); // copy value to provided pointer
      kvs_clock->ref_bits[i] = 1; // set ref bit to 1
      return 0; // return success
    }
  } // proceeding below is if key is not in cache ------

  //printf("[i] Key '%s' not found in cache. Fetching from base KVS.\n", key);
  int rc = kvs_base_get(kvs_clock->kvs_base, key, value); // probe return code
  if (rc == 0) { // if key not in base disk, add key-value pair to cache; should we count empty files? yes-rohan
    if (kvs_clock->size == kvs_clock->capacity) { // if cache full, replace a key-value pair using cock algorithm
      while (kvs_clock->ref_bits[kvs_clock->cursor]) { // find key-value pair with ref bit of 0
        kvs_clock->ref_bits[kvs_clock->cursor] = 0; // reset ref bit
        kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity; // move cursor
      }

      if (kvs_clock->dirty[kvs_clock->cursor]) { // if key-value pair is dirty, write back to base key-value disk
        kvs_base_set(kvs_clock->kvs_base, kvs_clock->keys[kvs_clock->cursor], kvs_clock->values[kvs_clock->cursor]);
        kvs_clock->dirty[kvs_clock->cursor] = false; // reset dirty flag
      }

      free(kvs_clock->keys[kvs_clock->cursor]); // free old key
      free(kvs_clock->values[kvs_clock->cursor]); // free old value
      kvs_clock->keys[kvs_clock->cursor] = strdup(key); // set new key
      kvs_clock->values[kvs_clock->cursor] = strdup(value); // set new value
      
      kvs_clock->dirty[kvs_clock->cursor] = false; // set to false because value is identical to the one on disk
      kvs_clock->ref_bits[kvs_clock->cursor] = 1; // set to 1 whenever a GET operation results in a cache miss

    } else { // if cache is not full
      kvs_clock->keys[kvs_clock->size] = strdup(key); // set new key
      kvs_clock->values[kvs_clock->size] = strdup(value); // set new value

      kvs_clock->dirty[kvs_clock->size] = false; // reset to false because the value is identical to the one on disk
      kvs_clock->ref_bits[kvs_clock->size] = 1; // set to 1 whenever a GET operation results in a cache miss

      kvs_clock->size++; // increase cache size
    }
  }
  
  return rc; // return return-code of kvs_base_get
}

int kvs_clock_flush(kvs_clock_t* kvs_clock) { // fn to flush COCK cache by persisting all dirty entries to disk and evicting all entries
  int result = 0; // initialize return code

  for (int i = 0; i < kvs_clock->size; ++i) { // iterate through all entries in cache
    if (kvs_clock->dirty[i]) { // if entry is dirty
      result |= kvs_base_set(kvs_clock->kvs_base, kvs_clock->keys[i], kvs_clock->values[i]); // was previously "result |=" bitwise OR operator to accumlate error return-codes
      kvs_clock->dirty[i] = false; // reset dirty flag
    }
    
    free(kvs_clock->keys[i]); // free key
    free(kvs_clock->values[i]); // free vals
    kvs_clock->keys[i] = NULL; // drop key arr
    kvs_clock->values[i] = NULL; // drop vals arr
    kvs_clock->ref_bits[i] = 0; // reset reference bit to 0
  }

  kvs_clock->size = 0; // reset size to 0 to clear it
  kvs_clock->cursor = 0; // reset cursor position to index 0

  return result; // return result
}
