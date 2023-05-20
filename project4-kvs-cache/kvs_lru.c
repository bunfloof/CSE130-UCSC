// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_lru.h"

#include <stdlib.h>
#include <string.h>

struct kvs_lru {
  kvs_base_t* kvs_base;
  char** keys;
  int capacity;
  int size;
};

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
  kvs_lru->kvs_base = kvs;
  kvs_lru->capacity = capacity;
  kvs_lru->size = 0;
  kvs_lru->keys = calloc(capacity, sizeof(char*));
  return kvs_lru;
}

void kvs_lru_free(kvs_lru_t** ptr) {
  for (int i = 0; i < (*ptr)->size; ++i) {
    free((*ptr)->keys[i]);
  }
  free((*ptr)->keys);
  free(*ptr);
  *ptr = NULL;
}

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      free(kvs_lru->keys[i]);
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      --kvs_lru->size;
      break;
    }
  }

  if (kvs_lru->size == kvs_lru->capacity) {
    free(kvs_lru->keys[0]);
    memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
    --kvs_lru->size;
  }
  kvs_lru->keys[kvs_lru->size] = strdup(key);
  ++kvs_lru->size;

  return kvs_base_set(kvs_lru->kvs_base, key, value);
}


int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {

      char* temp = kvs_lru->keys[i];
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      kvs_lru->keys[kvs_lru->size - 1] = temp;

      return kvs_base_get(kvs_lru->kvs_base, key, value);
    }
  }

  // if key is not found in the cache ----------------------------
  int rc = kvs_base_get(kvs_lru->kvs_base, key, value); 
  if (rc == 0) {  // if key is found in the underlying disk store
    if (kvs_lru->size == kvs_lru->capacity) {
      free(kvs_lru->keys[0]);
      memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
      --kvs_lru->size;
    }
    kvs_lru->keys[kvs_lru->size] = strdup(key);
    ++kvs_lru->size;
  }

  return rc;
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    free(kvs_lru->keys[i]);
  }
  kvs_lru->size = 0;
  return 0;
}
