#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_lru.h"
#include <stdbool.h> // newlt added to use bool data type

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct kvs_lru {
  kvs_base_t* kvs_base;
  char** keys;
  char** values; // added a values field
  bool* dirty; // changed int* to bool*
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
  kvs_lru->dirty = calloc(capacity, sizeof(bool)); // changed int to bool
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
      kvs_lru->dirty[i] = true;

      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      bool temp_dirty = kvs_lru->dirty[i];
      for (int j = i; j < kvs_lru->size - 1; ++j) {
        kvs_lru->keys[j] = kvs_lru->keys[j+1];
        kvs_lru->values[j] = kvs_lru->values[j+1];
        kvs_lru->dirty[j] = kvs_lru->dirty[j+1];
      }
      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;

      return 0;
    }
  }

  // if the key is not found in the cache and cache is full.
  if (kvs_lru->size == kvs_lru->capacity) {
    if (kvs_lru->dirty[0]) {
      int rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]);
      if (rc != 0) {
        return rc;
      }
      // increment replacement_count here, as a dirty pair was replaced
      kvs_lru->replacement_count++;
    }

    free(kvs_lru->keys[0]);
    free(kvs_lru->values[0]);
    
    memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(bool));
  } else {
    ++kvs_lru->size;
  }

  kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
  kvs_lru->values[kvs_lru->size - 1] = strdup(value);
  kvs_lru->dirty[kvs_lru->size - 1] = true;

  return 0; 
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      strcpy(value, kvs_lru->values[i]);

      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      bool temp_dirty = kvs_lru->dirty[i];
      for (int j = i; j < kvs_lru->size - 1; ++j) {
        kvs_lru->keys[j] = kvs_lru->keys[j+1];
        kvs_lru->values[j] = kvs_lru->values[j+1];
        kvs_lru->dirty[j] = kvs_lru->dirty[j+1];
      }
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
        // increment replacement_count here, as a dirty pair was replaced
        kvs_lru->replacement_count++;
      }

      free(kvs_lru->keys[0]);
      free(kvs_lru->values[0]);

      memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->dirty, kvs_lru->dirty + 1, (kvs_lru->size - 1) * sizeof(bool));
    } else {
      ++kvs_lru->size;
    }

    kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
    kvs_lru->values[kvs_lru->size - 1] = strdup(value);
    kvs_lru->dirty[kvs_lru->size - 1] = false;
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
      // If it's dirty, we can clean it after the write
      kvs_lru->dirty[i] = false;
    }
    free(kvs_lru->keys[i]);
    free(kvs_lru->values[i]);
  }
  kvs_lru->size = 0;
  // reset replacement_count after flushing
  kvs_lru->replacement_count = 0;

  return 0;
}

