// clang-format off
/*
WARNING: it appears that original source template for kvs_lru.c doesn't
doesn't have "#include <stdbool.h>" as compared to original source
template kvs_fifo.c. Should we still use bool for our dirty field to
maintain consistency?

This implemention uses int for now, but I'll create another kvs_lru.c
file with "#include <stdbool.h>" and bool* dirty to maintain 
consistency accross all cache implementations.
*/
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_lru.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct kvs_lru {
  kvs_base_t* kvs_base;
  char** keys;
  char** values; // added a values field
  int* dirty; // newly added dirty int
  int capacity;
  int size;
  int replacement_count; // added a replacement_count field
};

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
  kvs_lru->kvs_base = kvs;
  kvs_lru->capacity = capacity;
  kvs_lru->size = 0;
  kvs_lru->keys = calloc(capacity, sizeof(char*));
  kvs_lru->values = calloc(capacity, sizeof(char*)); // allocate memory for values
  kvs_lru->dirty = calloc(capacity, sizeof(int)); // newly added dirty mem
  kvs_lru->replacement_count = 0; // initialize replacement_count to 0
  return kvs_lru;
}

void kvs_lru_free(kvs_lru_t** ptr) {
  for (int i = 0; i < (*ptr)->size; ++i) {
    free((*ptr)->keys[i]);
    free((*ptr)->values[i]);
  }
  free((*ptr)->keys);
  free((*ptr)->values);
  free((*ptr)->dirty);
  free(*ptr);
  *ptr = NULL;
}

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      free(kvs_lru->values[i]); 
      kvs_lru->values[i] = strdup(value);
      kvs_lru->dirty[i] = 1;
      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      int temp_dirty = kvs_lru->dirty[i];
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->dirty + i, kvs_lru->dirty + i + 1, (kvs_lru->size - i - 1) * sizeof(int));

      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;

      return 0; // key was found in cache, no need to call kvs_base_set
    }
  }

  // if the key is not found in the cache and cache is full.
  if (kvs_lru->size == kvs_lru->capacity) {
    if (kvs_lru->dirty[0]) {
      int rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]);
      if (rc != 0) {
        return rc;
      }
    }

    free(kvs_lru->keys[0]);
    free(kvs_lru->values[0]);
    
    memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(int));
  } else {
    ++kvs_lru->size;
  }

  kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
  kvs_lru->values[kvs_lru->size - 1] = strdup(value);
  kvs_lru->dirty[kvs_lru->size - 1] = 1;

  return 0; // no need to update kvs_baes since no replacement in cache occurred
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      strcpy(value, kvs_lru->values[i]); // return value from cache
      // move the key, value, and dirty flag to the end of array to denote as most recently used
      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      int temp_dirty = kvs_lru->dirty[i];
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->dirty + i, kvs_lru->dirty + i + 1, (kvs_lru->size - i - 1) * sizeof(int));

      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;
      return 0;
    }
  }

  // if key is not found in the cache, get it from the base store.
  int rc = kvs_base_get(kvs_lru->kvs_base, key, value);
  if (rc == 0) {
    if (kvs_lru->size == kvs_lru->capacity) {
      if (kvs_lru->dirty[0]) { // if key dirty
        rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]);
        if (rc != 0) {
          return rc;
        }
      }
      free(kvs_lru->keys[0]);
      free(kvs_lru->values[0]);
      memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(int));
    } else {
      ++kvs_lru->size;
    }

    kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
    kvs_lru->values[kvs_lru->size - 1] = strdup(value);
    kvs_lru->dirty[kvs_lru->size - 1] = 0; // this value just read from the base store, it's not dirty
  }

  return rc;
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (kvs_lru->dirty[i]) {
      int rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[i], kvs_lru->values[i]);
      if (rc != 0) {
        return rc;
      }
    }
    free(kvs_lru->keys[i]);
    free(kvs_lru->values[i]);
  }
  kvs_lru->size = 0;

  return 0;
}
