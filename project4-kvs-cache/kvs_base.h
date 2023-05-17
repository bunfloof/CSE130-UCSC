#pragma once

#include <linux/limits.h>

#include "constants.h"

typedef struct kvs_base {
  char directory[PATH_MAX];
  int get_count;
  int set_count;
} kvs_base_t;

kvs_base_t* kvs_base_new(const char* directory);
void kvs_base_free(kvs_base_t** ptr);

int kvs_base_set(kvs_base_t* kvs, const char* key, const char* value);
int kvs_base_get(kvs_base_t* kvs, const char* key, char* value);
