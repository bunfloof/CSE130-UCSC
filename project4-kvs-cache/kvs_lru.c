// clang-format off
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_lru.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct kvs_lru {
  kvs_base_t* kvs_base;
  char** keys;
  char** values; // added a values field
  int capacity;
  int size;
  int replacement_count; // added a replacement_count field
};

/*
BUT
CAN NOT DO REPLACEMENT_COUNT WITHOUT GETTER FUNC, 
CAN NOT MODIFY kvs_base.c. NEED TO PREVENT PREVENT kvs_base.c 
FROM ICNREMENTING SET COUNTS.
*/

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
  kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
  kvs_lru->kvs_base = kvs;
  kvs_lru->capacity = capacity;
  kvs_lru->size = 0;
  kvs_lru->keys = calloc(capacity, sizeof(char*));
  kvs_lru->values = calloc(capacity, sizeof(char*)); // allocate memory for values
  kvs_lru->replacement_count = 0;  // initialize replacement_count to 0
  return kvs_lru;
}

void kvs_lru_free(kvs_lru_t** ptr) {
  for (int i = 0; i < (*ptr)->size; ++i) {
    free((*ptr)->keys[i]);
    free((*ptr)->values[i]); // free each value
  }
  free((*ptr)->keys);
  free((*ptr)->values); // free the values array
  free(*ptr);
  *ptr = NULL;
}

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      free(kvs_lru->values[i]); 
      kvs_lru->values[i] = strdup(value);
      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      //printf("key was found in cache");
      return 0; // key was found in cache, no need to call kvs_base_set
    }
  } // proceed below if key not found in cache ----------

  if (kvs_lru->size == kvs_lru->capacity) { // if cache full, replace an entry
    free(kvs_lru->keys[0]);
    free(kvs_lru->values[0]);
    memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
    memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
    int rc = kvs_base_set(kvs_lru->kvs_base, key, value); // key not found in cache, so set in kvs_base
    if (rc != 0) { // error handilng
      return rc;
    }
    kvs_lru->keys[kvs_lru->size - 1] = strdup(key); // last position in array now free
    kvs_lru->values[kvs_lru->size - 1] = strdup(value);
    return 0;
  } // proceed below if key not found in cache AND cache is not full, so just add the new key value pair ------
  
  //printf("cache not full and ket found in cache!!");
  kvs_lru->keys[kvs_lru->size] = strdup(key);
  kvs_lru->values[kvs_lru->size] = strdup(value); 
  ++kvs_lru->size;
  return 0; // no need to update kvs_baes since no replacement in cache occurred
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    if (strcmp(kvs_lru->keys[i], key) == 0) {
      strcpy(value, kvs_lru->values[i]); // return the value from the cache
      // move the key and value to the end of the arrsy to denote it as the most recently used
      char* temp_key = kvs_lru->keys[i];
      char* temp_value = kvs_lru->values[i];
      memmove(kvs_lru->keys + i, kvs_lru->keys + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      memmove(kvs_lru->values + i, kvs_lru->values + i + 1, (kvs_lru->size - i - 1) * sizeof(char*));
      kvs_lru->keys[kvs_lru->size - 1] = temp_key;
      kvs_lru->values[kvs_lru->size - 1] = temp_value;
      return 0;
    }
  }

  // if key is not found in the cache
  int rc = kvs_base_get(kvs_lru->kvs_base, key, value); 
  if (rc == 0) { // if key is found in the underlying disk store
    if (kvs_lru->size == kvs_lru->capacity) {
      free(kvs_lru->keys[0]);
      free(kvs_lru->values[0]); // free the old value
      memmove(kvs_lru->keys, kvs_lru->keys + 1, (kvs_lru->size - 1) * sizeof(char*));
      memmove(kvs_lru->values, kvs_lru->values + 1, (kvs_lru->size - 1) * sizeof(char*));
      --kvs_lru->size;
    }
    kvs_lru->keys[kvs_lru->size] = strdup(key);
    kvs_lru->values[kvs_lru->size] = strdup(value); // store the value in the cache
    ++kvs_lru->size;
  }

  return rc;
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
  for (int i = 0; i < kvs_lru->size; ++i) {
    free(kvs_lru->keys[i]);
    free(kvs_lru->values[i]); // free each value
  }
  kvs_lru->size = 0;
  return 0;
}
