// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_fifo.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct kvs_fifo {
  // TODO: add necessary variables
  kvs_base_t* kvs_base;
  int capacity;
  char **keys; // arr to store keys
  int size; // current size of KVS
  int head; // index of oldest (first-in) key
  int tail; // index of newest (last-in) key 
};

kvs_fifo_t* kvs_fifo_new(kvs_base_t* kvs, int capacity) {
  kvs_fifo_t* kvs_fifo = malloc(sizeof(kvs_fifo_t));
  kvs_fifo->kvs_base = kvs;
  kvs_fifo->capacity = capacity;
  // TODO: initialize other variables
  kvs_fifo->size = 0;
  kvs_fifo->head = 0;
  kvs_fifo->tail = 0;
  kvs_fifo->keys = malloc(capacity * sizeof(char*)); // allocate memory for keys
  return kvs_fifo;
}

void kvs_fifo_free(kvs_fifo_t** ptr) {
  // TODO: free dynamically allocated memory
  for (int i = 0; i < (*ptr)->size; ++i) { // free keys array and KVS
    free((*ptr)->keys[i]);
  }
  free((*ptr)->keys);
  free(*ptr);
  *ptr = NULL;
}

int kvs_fifo_set(kvs_fifo_t* kvs_fifo, const char* key, const char* value) {
  // TODO: implement this function
  if (kvs_fifo->size == kvs_fifo->capacity) { // if KVS is full, evict oldest key
    if (kvs_base_set(kvs_fifo->kvs_base, kvs_fifo->keys[kvs_fifo->head], NULL) == FAILURE) {
      return FAILURE;
    }
    free(kvs_fifo->keys[kvs_fifo->head]);
    kvs_fifo->head = (kvs_fifo->head + 1) % kvs_fifo->capacity;
    kvs_fifo->size--;
  }
  
  kvs_fifo->keys[kvs_fifo->tail] = strdup(key); // add new key to the KVS
  kvs_fifo->tail = (kvs_fifo->tail + 1) % kvs_fifo->capacity;
  kvs_fifo->size++;

  return kvs_base_set(kvs_fifo->kvs_base, key, value); // set key-value pair in base KVS
}

int kvs_fifo_get(kvs_fifo_t* kvs_fifo, const char* key, char* value) {
  // TODO: implement this function
  for (int i = 0; i < kvs_fifo->size; ++i) {
    int index = (kvs_fifo->head + i) % kvs_fifo->capacity;
    if (strcmp(kvs_fifo->keys[index], key) == 0) {
      return kvs_base_get(kvs_fifo->kvs_base, key, value); // key is in cache, get it from base KVS
    }
  }
  return kvs_base_get(kvs_fifo->kvs_base, key, value); // key is not in cache, fetch it from the disk
}

int kvs_fifo_flush(kvs_fifo_t* kvs_fifo) {
  // TODO: implement this function
  for (int i = 0; i < kvs_fifo->size; ++i) { // evict all keys from the KVS and reset the indices
    if (kvs_base_set(kvs_fifo->kvs_base, kvs_fifo->keys[(kvs_fifo->head + i) % kvs_fifo->capacity], NULL) == FAILURE) {
      return FAILURE;
    }
    free(kvs_fifo->keys[(kvs_fifo->head + i) % kvs_fifo->capacity]);
  }
  kvs_fifo->size = 0;
  kvs_fifo->head = 0;
  kvs_fifo->tail = 0;
  return SUCCESS;
}
