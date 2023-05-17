#pragma once

#include "kvs_base.h"
#include "kvs_clock.h"
#include "kvs_fifo.h"
#include "kvs_lru.h"

/**
 * `kvs_replacement_policy` represents a cache policy.
 */
typedef enum {
  KVS_CACHE_NONE,
  KVS_CACHE_FIFO,
  KVS_CACHE_CLOCK,
  KVS_CACHE_LRU,
} kvs_replacement_policy;

typedef struct kvs {
  kvs_base_t* kvs_base;
  kvs_replacement_policy policy;
  int get_count;
  int set_count;
  union {
    kvs_fifo_t* fifo;
    kvs_clock_t* clock;
    kvs_lru_t* lru;
  };
} kvs_t;

kvs_t* kvs_new(const char* directory, kvs_replacement_policy policy,
               int capacity);

void kvs_free(kvs_t** ptr);

int kvs_get(kvs_t* kvs, const char* key, char* value);
int kvs_set(kvs_t* kvs, const char* key, const char* value);
int kvs_flush(kvs_t* kvs);
