// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_clock.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct kvs_clock {
  kvs_base_t* kvs_base;
  int capacity;
  char **keys;
  char **values;
  int *ref_bits;
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
  free(*ptr);
  *ptr = NULL;
}

int kvs_clock_set(kvs_clock_t* kvs_clock, const char* key, const char* value) {
  if (kvs_clock->size == kvs_clock->capacity) {
    free(kvs_clock->keys[kvs_clock->cursor]);
    free(kvs_clock->values[kvs_clock->cursor]);
  } else {
    kvs_clock->size++;
  }

  kvs_clock->keys[kvs_clock->cursor] = strdup(key);
  kvs_clock->values[kvs_clock->cursor] = strdup(value);
  kvs_clock->ref_bits[kvs_clock->cursor] = 1;

  kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity;

  return kvs_base_set(kvs_clock->kvs_base, key, value);
}

int kvs_clock_get(kvs_clock_t* kvs_clock, const char* key, char* value) {
  for (int i = 0; i < kvs_clock->size; ++i) {
    if (strcmp(kvs_clock->keys[i], key) == 0) {
      strcpy(value, kvs_clock->values[i]);
      kvs_clock->ref_bits[i] = 1;
      return 0;
    }
  }

  int rc = kvs_base_get(kvs_clock->kvs_base, key, value);
  if (rc == 0 && value[0] != '\0') {
    if (kvs_clock->size == kvs_clock->capacity) {
      while (kvs_clock->ref_bits[kvs_clock->cursor]) {
        kvs_clock->ref_bits[kvs_clock->cursor] = 0;
        kvs_clock->cursor = (kvs_clock->cursor + 1) % kvs_clock->capacity;
      }

      free(kvs_clock->keys[kvs_clock->cursor]);
      free(kvs_clock->values[kvs_clock->cursor]);
      kvs_clock->keys[kvs_clock->cursor] = strdup(key);
      kvs_clock->values[kvs_clock->cursor] = strdup(value);
      kvs_clock->ref_bits[kvs_clock->cursor] = 1;
    } else {
      kvs_clock->keys[kvs_clock->size] = strdup(key);
      kvs_clock->values[kvs_clock->size] = strdup(value);
      kvs_clock->ref_bits[kvs_clock->size] = 1;
      kvs_clock->size++;
    }
  }
  
  return rc;
}

int kvs_clock_flush(kvs_clock_t* kvs_clock) {
  for (int i = 0; i < kvs_clock->size; ++i) {
    free(kvs_clock->keys[i]);
    free(kvs_clock->values[i]);
    kvs_clock->keys[i] = NULL;
    kvs_clock->values[i] = NULL;
    kvs_clock->ref_bits[i] = 0;
  }

  kvs_clock->size = 0;
  kvs_clock->cursor = 0;

  return 0;
}
