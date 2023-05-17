#include "kvs_lru.h"

#include <stdlib.h>

struct kvs_lru {
  // TODO: add necessary variables
  kvs_base_t* kvs_base;
  int capacity;
};

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
  kvs_lru->kvs_base = kvs;
  kvs_lru->capacity = capacity;

  // TODO: initialize other variables

  return kvs_lru;
}

void kvs_lru_free(kvs_lru_t** ptr) {
  // TODO: free dynamically allocated memory

  free(*ptr);
  *ptr = NULL;
}

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) {
  // TODO: implement this function
  return FAILURE;
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  // TODO: implement this function
  return FAILURE;
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  // TODO: implement this function
  return FAILURE;
}
