#include "kvs.h"

#include <stdlib.h>

kvs_t* kvs_new(const char* directory, kvs_replacement_policy policy,
               int capacity) {
  kvs_t* instance = malloc(sizeof(kvs_t));
  instance->kvs_base = kvs_base_new(directory);
  instance->policy = policy;
  instance->get_count = 0;
  instance->set_count = 0;
  switch (policy) {
    case KVS_CACHE_NONE:
      break;
    case KVS_CACHE_FIFO:
      instance->fifo = kvs_fifo_new(instance->kvs_base, capacity);
      break;
    case KVS_CACHE_CLOCK:
      instance->clock = kvs_clock_new(instance->kvs_base, capacity);
      break;
    case KVS_CACHE_LRU:
      instance->lru = kvs_lru_new(instance->kvs_base, capacity);
      break;
  }
  return instance;
}

void kvs_free(kvs_t** ptr) {
  kvs_t* instance = *ptr;
  switch (instance->policy) {
    case KVS_CACHE_NONE:
      break;
    case KVS_CACHE_FIFO:
      kvs_fifo_free(&instance->fifo);
      break;
    case KVS_CACHE_CLOCK:
      kvs_clock_free(&instance->clock);
      break;
    case KVS_CACHE_LRU:
      kvs_lru_free(&instance->lru);
      break;
  }
  kvs_base_free(&instance->kvs_base);
  free(instance);
  *ptr = NULL;
}

int kvs_get(kvs_t* kvs, const char* key, char* value) {
  kvs->get_count += 1;
  switch (kvs->policy) {
    case KVS_CACHE_NONE:
      return kvs_base_get(kvs->kvs_base, key, value);
    case KVS_CACHE_FIFO:
      return kvs_fifo_get(kvs->fifo, key, value);
    case KVS_CACHE_CLOCK:
      return kvs_clock_get(kvs->clock, key, value);
    case KVS_CACHE_LRU:
      return kvs_lru_get(kvs->lru, key, value);
  }
  return FAILURE;  // impossible
}

int kvs_set(kvs_t* kvs, const char* key, const char* value) {
  kvs->set_count += 1;
  switch (kvs->policy) {
    case KVS_CACHE_NONE:
      return kvs_base_set(kvs->kvs_base, key, value);
    case KVS_CACHE_FIFO:
      return kvs_fifo_set(kvs->fifo, key, value);
    case KVS_CACHE_CLOCK:
      return kvs_clock_set(kvs->clock, key, value);
    case KVS_CACHE_LRU:
      return kvs_lru_set(kvs->lru, key, value);
  }
  return FAILURE;  // impossible
}

int kvs_flush(kvs_t* kvs) {
  switch (kvs->policy) {
    case KVS_CACHE_NONE:
      // no need to flush for KVS_CACHE_NONE
      return SUCCESS;
    case KVS_CACHE_FIFO:
      return kvs_fifo_flush(kvs->fifo);
    case KVS_CACHE_CLOCK:
      return kvs_clock_flush(kvs->clock);
    case KVS_CACHE_LRU:
      return kvs_lru_flush(kvs->lru);
  }
  return SUCCESS;
}
