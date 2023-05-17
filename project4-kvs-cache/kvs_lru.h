#pragma once

#include "kvs_base.h"

struct kvs_lru;
typedef struct kvs_lru kvs_lru_t;

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity);
void kvs_lru_free(kvs_lru_t** ptr);

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value);
int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value);
int kvs_lru_flush(kvs_lru_t* kvs_lru);
#pragma once
