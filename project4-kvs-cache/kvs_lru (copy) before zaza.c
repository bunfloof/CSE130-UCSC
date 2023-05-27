#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include <stdbool.h>  // newlt added to use bool data type
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kvs_lru.h"

struct kvs_lru {
  kvs_base_t* kvs_base;
  char** keys;
  char** values;  // added a values field
  bool* dirty;    // changed int* to bool*
  int capacity;
  int size;
  int replacement_count;  // added a replacement_count field
};

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
  kvs_lru->kvs_base = kvs;
  kvs_lru->capacity = capacity;
  kvs_lru->size = 0;
  kvs_lru->keys = calloc(capacity, sizeof(char*));
  kvs_lru->values =
      calloc(capacity, sizeof(char*));  // allocate memory for values
  kvs_lru->dirty = calloc(capacity, sizeof(bool));  // changed int to bool
  kvs_lru->replacement_count = 0;  // initialize replacement_count to 0
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
      bool temp_dirty = kvs_lru->dirty[i];  // changed int to bool
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1,
              (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1,
              (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->dirty + i, kvs_lru->dirty + i + 1,
              (kvs_lru->size - i - 1) * sizeof(bool));  // changed int to bool

      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      kvs_lru->dirty[kvs_lru->size - 1] = temp_dirty;

      return 0;  // key was found in cache, no need to call kvs_base_set
    }
  }

  // if the key is not found in the cache and cache is full.
  if (kvs_lru->size == kvs_lru->capacity) {
    if (kvs_lru->dirty[0]) {
      int rc =
          kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0], kvs_lru->values[0]);
      if (rc != 0) {
        return rc;
      }
    }

    free(kvs_lru->keys[0]);
    free(kvs_lru->values[0]);

    memmove(kvs_lru->keys, kvs_lru->keys + 1,
            (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->values, kvs_lru->values + 1,
            (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->dirty, kvs_lru->dirty + 1,
            (kvs_lru->size - 1) * sizeof(bool));  // changed int to bool
  } else {
    ++kvs_lru->size;
  }

  kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
  kvs_lru->values[kvs_lru->size - 1] = strdup(value);
  kvs_lru->dirty[kvs_lru->size - 1] = true;

  return 0;  // no need to update kvs_baes since no replacement in cache
             // occurred
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      strcpy(value, kvs_lru->values[i]);  // return value from cache
      // move the key, value, and dirty flag to the end of array to denote as
      // most recently used
      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      bool temp_dirty = kvs_lru->dirty[i];  // changed int to bool
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1,
              (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1,
              (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->dirty + i, kvs_lru->dirty + i + 1,
              (kvs_lru->size - i - 1) * sizeof(bool));  // changed int to bool

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
      if (kvs_lru->dirty[0]) {  // if key dirty
        rc = kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[0],
                          kvs_lru->values[0]);
        if (rc != 0) {
          return rc;
        }
      }
      free(kvs_lru->keys[0]);
      free(kvs_lru->values[0]);
      memmove(kvs_lru->keys, kvs_lru->keys + 1,
              (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->values, kvs_lru->values + 1,
              (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->dirty, kvs_lru->dirty + 1,
              (kvs_lru->size - 1) * sizeof(bool));  // changed int to bool
    } else {
      ++kvs_lru->size;
    }

    kvs_lru->keys[kvs_lru->size - 1] = strdup(key);
    kvs_lru->values[kvs_lru->size - 1] = strdup(value);
    kvs_lru->dirty[kvs_lru->size - 1] =
        false;  // this value just read from the base store, it's not dirty
  }

  return rc;
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (kvs_lru->dirty[i]) {
      int rc =
          kvs_base_set(kvs_lru->kvs_base, kvs_lru->keys[i], kvs_lru->values[i]);
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
