#include "kvs_clock.h"

#include <stdlib.h>

struct kvs_clock {
  // TODO: add necessary variables
  kvs_base_t* kvs_base;
  int capacity;
};

kvs_clock_t* kvs_clock_new(kvs_base_t* kvs, int capacity) {
  kvs_clock_t* kvs_clock = malloc(sizeof(kvs_clock_t));
  kvs_clock->kvs_base = kvs;
  kvs_clock->capacity = capacity;

  // TODO: initialize other variables

  return kvs_clock;
}

void kvs_clock_free(kvs_clock_t** ptr) {
  // TODO: free dynamically allocated memory

  free(*ptr);
  *ptr = NULL;
}

int kvs_clock_set(kvs_clock_t* kvs_clock, const char* key, const char* value) {
  // TODO: implement this function
  return FAILURE;
}

int kvs_clock_get(kvs_clock_t* kvs_clock, const char* key, char* value) {
  // TODO: implement this function
  return FAILURE;
}

int kvs_clock_flush(kvs_clock_t* kvs_clock) {
  // TODO: implement this function
  return FAILURE;
}
