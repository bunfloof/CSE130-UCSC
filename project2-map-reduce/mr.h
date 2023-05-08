#pragma once

#include <stddef.h>

#include "kvlist.h"

typedef void (*mapper_t)(kvpair_t* kv, kvlist_t* output);
typedef void (*reducer_t)(char* key, kvlist_t* list, kvlist_t* output);

void map_reduce(mapper_t mapper, size_t num_mapper, reducer_t reducer,
                size_t num_reducer, kvlist_t* input, kvlist_t* output);
