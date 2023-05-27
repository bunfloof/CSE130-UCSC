// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_clock.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct kvs_clock {
  kvs_base_t* kvs_base;
  int capacity;
  char **keys;
  char **values;
  int *ref_bits;
  bool *dirty; // newly added dirty array
  int size; 
  int cursor;
};

kvs_clock_t* kvs_clock_new(kvs_base_t* kvs, int capacity) {
  kvs_clock_t* kvs_clock = malloc(sizeof(kvs_clock_t));
  kvs_clock->kvs_base = kvs;
  kvs_clock->capacity = capacity;
  kvs_clock->keys = calloc(capacity, sizeof(char*));
  kvs_clock->values = calloc(capacity, sizeof(char*));
  kvs_clock->ref_bits = calloc(capacity, sizeof(int));
  kvs_clock->dirty = calloc(capacity, sizeof(bool)); // newly initialized dirt array
  kvs_clock->size = 0;
  kvs_clock->cursor = 0;
  return kvs_clock;
}

void kvs_clock_free(kvs_clock_t** ptr) {
  for (int i = 0; i < (*ptr)->size; ++i) {
    free((*ptr)->keys[i]);
    free((*ptr)->values[i]);
  }
  free((*ptr)->keys);
  free((*ptr)->values);
  free((*ptr)->ref_bits);
  free((*ptr)->dirty); // newly free dirty array
  free(*ptr);
  *ptr = NULL;
}

int kvs_clock_set(kvs_clock_t* kvs_clock, const char* key, const char* value) {
  for (int i = 0; i < kvs_clock->size; ++i) {
    if (strcmp(kvs_clock->keys[i], key) == 0) { // if key in cache, update value
      printf("🔵 Setting key '%s' which already exists in the cache at index %d\n", key, i);
      free(kvs_clock->values[i]); // free old value
      kvs_clock->values[i] = strdup(value); // set new value
      kvs_clock->ref_bits[i] = 1; // set ref bit to 1
      kvs_clock->dirty[i] = true; // mark as dirty
      return 0;
    }
  } // proceeding below if key not in cache ---------------------

  if (kvs_clock->size < kvs_clock->capacity) { // if cache not full, add key-value pair to cache
    printf("🔵 Cache is not full. Adding key '%s' at index %d\n", key, kvs_clock->size);
    kvs_clock->keys[kvs_clock->size] = strdup(key); // add key
    kvs_clock->values[kvs_clock->size] = strdup(value); // add value
    kvs_clock->dirty[kvs_clock->size] = true; // mark as dirty
    kvs_clock->size++; // increase cache size
  } else { // if cache is full, replace key-value pair using cock algorithm
    while (kvs_clock->ref_bits[kvs_clock->cursor]) { // find key-value pair with ref bit of 0
      printf("🔵 Cache is full. Replacing key '%s' at index %d\n", key, kvs_clock->cursor);
      kvs_clock->ref_bits[kvs_clock->cursor] = 0; // reset ref bit
      kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity; // move cursor
    }

    if (kvs_clock->dirty[kvs_clock->cursor]) { // if key-value pair is dirty, write back to base key-value store
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
  
  return 0;
}

int kvs_clock_get(kvs_clock_t* kvs_clock, const char* key, char* value) {
  for (int i = 0; i < kvs_clock->size; ++i) {
    if (strcmp(kvs_clock->keys[i], key) == 0) { // if key is in cache
      printf("🔵 Key '%s' found in cache at index %d\n", key, i);
      strcpy(value, kvs_clock->values[i]); // copy value to buffer
      kvs_clock->ref_bits[i] = 1; // set ref bit to 1
      return 0;
    }
  } // proceeding below is if key is not in cache -------------------------

  printf("🔵 Key '%s' not found in cache. Fetching from base KVS.\n", key);
  int rc = kvs_base_get(kvs_clock->kvs_base, key, value);
  if (rc == 0) { // if key not in base key-value store, add key-value pair to cache; should we count empty files???
    if (kvs_clock->size == kvs_clock->capacity) { // if cache full, replace a key-value pair using cock algorithm
      while (kvs_clock->ref_bits[kvs_clock->cursor]) { // find key-value pair with ref bit of 0
        kvs_clock->ref_bits[kvs_clock->cursor] = 0; // reset ref bit
        kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity; // move cursor
      }

      if (kvs_clock->dirty[kvs_clock->cursor]) { // if key-value pair is dirty, write back to base key-value store
        kvs_base_set(kvs_clock->kvs_base, kvs_clock->keys[kvs_clock->cursor], kvs_clock->values[kvs_clock->cursor]);
        kvs_clock->dirty[kvs_clock->cursor] = false; // reset dirty flag
      }

      free(kvs_clock->keys[kvs_clock->cursor]); // free old key
      free(kvs_clock->values[kvs_clock->cursor]); // free old value
      kvs_clock->keys[kvs_clock->cursor] = strdup(key); // set new key
      kvs_clock->values[kvs_clock->cursor] = strdup(value); // set new value
      
      kvs_clock->dirty[kvs_clock->cursor] = false; // set to false because value is identical to the one on disk
      kvs_clock->ref_bits[kvs_clock->cursor] = 1;  // should we be set to 1 whenever a GET operation results in a cache miss???

    } else { // if cache not full
      kvs_clock->keys[kvs_clock->size] = strdup(key); // set new key
      kvs_clock->values[kvs_clock->size] = strdup(value); // set new value

      kvs_clock->dirty[kvs_clock->size] = false; // reset to false because the value is identical to the one on disk
      kvs_clock->ref_bits[kvs_clock->size] = 1; // should we be set to 1???

      kvs_clock->size++; // increase cache size
    }
  }
  
  return rc;
}

int kvs_clock_flush(kvs_clock_t* kvs_clock) {
  int result = 0;

  for (int i = 0; i < kvs_clock->size; ++i) {
    if (kvs_clock->dirty[i]) {
      result |= kvs_base_set(kvs_clock->kvs_base, kvs_clock->keys[i], kvs_clock->values[i]);
      kvs_clock->dirty[i] = false; // reset dirty flag
    }
    
    free(kvs_clock->keys[i]);
    free(kvs_clock->values[i]);
    kvs_clock->keys[i] = NULL;
    kvs_clock->values[i] = NULL;
    kvs_clock->ref_bits[i] = 0;
  }

  kvs_clock->size = 0;
  kvs_clock->cursor = 0;

  return result;
}
