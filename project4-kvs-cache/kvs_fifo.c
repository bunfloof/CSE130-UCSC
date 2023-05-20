// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_fifo.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct kvs_fifo {
  kvs_base_t* kvs_base;
  int capacity;
  char **keys;
  char **values;
  int size; 
  int head; 
  int tail;
};

kvs_fifo_t* kvs_fifo_new(kvs_base_t* kvs, int capacity) {
  kvs_fifo_t* kvs_fifo = malloc(sizeof(kvs_fifo_t));
  kvs_fifo->kvs_base = kvs;
  kvs_fifo->capacity = capacity;
  kvs_fifo->keys = calloc(capacity, sizeof(char*));
  kvs_fifo->values = calloc(capacity, sizeof(char*));
  kvs_fifo->size = 0;
  kvs_fifo->head = 0;
  kvs_fifo->tail = 0;
  return kvs_fifo;
}

void kvs_fifo_free(kvs_fifo_t** ptr) {
  for (int i = 0; i < (*ptr)->size; ++i) {
    free((*ptr)->keys[i]);
    free((*ptr)->values[i]);
  }
  free((*ptr)->keys);
  free((*ptr)->values);
  free(*ptr);
  *ptr = NULL;
}

int kvs_fifo_set(kvs_fifo_t* kvs_fifo, const char* key, const char* value) {
  if (kvs_fifo->capacity == 0) { // case for capacity 0
    return kvs_base_set(kvs_fifo->kvs_base, key, value);
  }
  // look for the key in the cache
  for (int i = 0; i < kvs_fifo->size; ++i) {
    int index = (kvs_fifo->head + i) % kvs_fifo->capacity;
    if (strcmp(kvs_fifo->keys[index], key) == 0) {
      // found the key in the cache, update it's value
      free(kvs_fifo->values[index]);
      kvs_fifo->values[index] = strdup(value);
      return kvs_base_set(kvs_fifo->kvs_base, key, value);
    }
  }
  // the key was not in the cache
  if (kvs_fifo->size == kvs_fifo->capacity) { // the cache is full, evict the head
    printf("Cache is full. Evicting %s.\n", kvs_fifo->keys[kvs_fifo->head]);
    free(kvs_fifo->keys[kvs_fifo->head]);
    free(kvs_fifo->values[kvs_fifo->head]);
    kvs_fifo->head = (kvs_fifo->head + 1) % kvs_fifo->capacity;
    kvs_fifo->size--;
  }

  printf("Setting %s in cache.\n", key);
  kvs_fifo->keys[kvs_fifo->tail] = strdup(key);
  kvs_fifo->values[kvs_fifo->tail] = strdup(value);
  kvs_fifo->tail = (kvs_fifo->tail + 1) % kvs_fifo->capacity;
  kvs_fifo->size++;
  printf("Cache now contains %d items.\n", kvs_fifo->size);

  return kvs_base_set(kvs_fifo->kvs_base, key, value);
}

int kvs_fifo_get(kvs_fifo_t* kvs_fifo, const char* key, char* value) {
  if (kvs_fifo->capacity == 0) { // case for capacity 0
    return kvs_base_get(kvs_fifo->kvs_base, key, value);
  }
  for (int i = 0; i < kvs_fifo->size; ++i) {
    int index = (kvs_fifo->head + i) % kvs_fifo->capacity;
    if (strcmp(kvs_fifo->keys[index], key) == 0) {
      printf("Cache hit for %s.\n", key);
      strcpy(value, kvs_fifo->values[index]);
      return 0;
    }
  }
  
  printf("Cache miss for %s.\n", key);
  int rc = kvs_base_get(kvs_fifo->kvs_base, key, value);
  if (rc == 0 && value[0] != '\0') {
    // check if key is already in cache
    for (int i = 0; i < kvs_fifo->size; ++i) {
      int index = (kvs_fifo->head + i) % kvs_fifo->capacity;
      if (strcmp(kvs_fifo->keys[index], key) == 0) {
        return rc;
      }
    }

    // if key is not in cache, add ir
    if (kvs_fifo->size == kvs_fifo->capacity) { // store in cache without calling kvs_fifo_set
      free(kvs_fifo->keys[kvs_fifo->head]);
      free(kvs_fifo->values[kvs_fifo->head]);
      kvs_fifo->head = (kvs_fifo->head + 1) % kvs_fifo->capacity;
      kvs_fifo->size--;
    }
    kvs_fifo->keys[kvs_fifo->tail] = strdup(key);
    kvs_fifo->values[kvs_fifo->tail] = strdup(value);
    kvs_fifo->tail = (kvs_fifo->tail + 1) % kvs_fifo->capacity;
    kvs_fifo->size++;
  }
  return rc;
}


int kvs_fifo_flush(kvs_fifo_t* kvs_fifo) {
  while (kvs_fifo->size > 0) {
    free(kvs_fifo->keys[kvs_fifo->head]);
    free(kvs_fifo->values[kvs_fifo->head]);
    kvs_fifo->head = (kvs_fifo->head + 1) % kvs_fifo->capacity;
    kvs_fifo->size--;
  }
  return 0;
}
