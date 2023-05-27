/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project4
 * kvs_fifo.c
 * key-value FIFO cache policy
 *
 * Notes:
 * - When file is empty or '\0', GET still adds to cache.
 * - Cache is a circular array.
 *
 * Usage:
 * See comments near functions.
 *
 * Citations:
 * - Omkar TA
 * - Dongjing tutor
 * - Rohan tutor
 * - https://git.ucsc.edu/jma363/cse101-winter2022/-/blob/main/pa4/List.c
 *
 *********************************************************************************/
#define _GNU_SOURCE
// #define _GNU_SOURCE needed to use POSIX-compliant strdup
#include "kvs_fifo.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// structs --------------------------------------------------------------------

struct kvs_fifo {
  kvs_base_t* kvs_base;  // base key-value store
  int capacity;          // maximum capacity of FIFO cache
  char** keys;           // dynamic array of keys
  char** values;         // dynamic arr of values
  bool* dirty;           // bool arr to track dirty key-value pair
  int size;              // current size of cache
  int head;              // index of first element in cache
  int tail;              // index of last element cache
};

// Constructors-Destructors ---------------------------------------------------

kvs_fifo_t* kvs_fifo_new(kvs_base_t* kvs, int capacity) {
  kvs_fifo_t* kvs_fifo = malloc(sizeof(kvs_fifo_t));  // base of key-value store
  kvs_fifo->kvs_base = kvs;       // set base key-value store
  kvs_fifo->capacity = capacity;  // set capacity of cache
  kvs_fifo->keys =
      calloc(capacity, sizeof(char*));  // allocate memory for keys arr
  kvs_fifo->values =
      calloc(capacity, sizeof(char*));  // alocate mem for vals arr
  kvs_fifo->dirty =
      calloc(capacity, sizeof(bool));  // newly intizlized dirty arr
  kvs_fifo->size = 0;                  // initialize size of 0
  kvs_fifo->head = 0;                  // set head to beginning
  kvs_fifo->tail = 0;                  // set tail to beginning
  return kvs_fifo;                     // return newly created cache
}

void kvs_fifo_free(kvs_fifo_t** ptr) {  // memory unsafe function to free memory
                                        // from FIFO cache
  for (int i = 0; i < (*ptr)->size;
       ++i) {                 // iterate over each element in cache
    free((*ptr)->keys[i]);    // free each key
    free((*ptr)->values[i]);  // free each val
  }
  free((*ptr)->keys);    // free mem allocated for keys arr
  free((*ptr)->values);  // free mem allocated for vals arr
  free((*ptr)->dirty);   // free mem allocated for dirty arr
  free(*ptr);            // free memory allocated for kvs_fifo
  *ptr = NULL;           // drop pointer
}

// Manipulation and access procedures -----------------------------------------

int kvs_fifo_set(kvs_fifo_t* kvs_fifo, const char* key,
                 const char* value) {  // fn to add key-value pair to FIFO cache
  if (kvs_fifo->capacity == 0)
    return kvs_base_set(
        kvs_fifo->kvs_base, key,
        value);  // case for capacity 0 set val in disk and return result
  // proceed below to look in cache ------

  for (int i = 0; i < kvs_fifo->size; ++i) {  // iterate through cache
    int index =
        (kvs_fifo->head + i) %
        kvs_fifo->capacity;  // calculate index based on head and current loop
                             // counter wrapped around cache capacity
    if (strcmp(kvs_fifo->keys[index], key) ==
        0) {  // if key found in cache, then update its value
      free(kvs_fifo->values[index]);  // free old val associated with key
      kvs_fifo->values[index] =
          strdup(value);  // update val associated with key in cache
      kvs_fifo->dirty[index] =
          true;  // mark key-value as dirty because we updated val
      return 0;  // return success without updating the disk store
    }
  }  // proceed below if the key was not found in the cache ------

  if (kvs_fifo->size ==
      kvs_fifo->capacity) {  // if the cache is full, evict the head
    if (kvs_fifo->dirty[kvs_fifo->head]) {  // if the entry being evicted is
                                            // dirty, persist it to disk
      kvs_base_set(kvs_fifo->kvs_base, kvs_fifo->keys[kvs_fifo->head],
                   kvs_fifo->values[kvs_fifo->head]);
    }  // proceed below if cache is full ------

    free(kvs_fifo->keys[kvs_fifo->head]);  // free key and val of evicted entry
    free(kvs_fifo->values[kvs_fifo->head]);

    kvs_fifo->head = (kvs_fifo->head + 1) %
                     kvs_fifo->capacity;  // update head index to point to next
                                          // entry in circular arr cache
    kvs_fifo->size--;  // decrease sizeof cache because we evicted an entry
    // no longer call kvs_base_set here for every eviction, only when the entry
    // is dirty
  }

  // when we add a new entry to the cache, it's initially dirty because it
  // hasn't been persisted yet
  kvs_fifo->keys[kvs_fifo->tail] =
      strdup(key);  // add new key-value pair to tail of cache
  kvs_fifo->values[kvs_fifo->tail] = strdup(value);
  kvs_fifo->dirty[kvs_fifo->tail] =
      true;  // mark new key-value pair as dirt because it hasn't persisted to
             // disk yet
  kvs_fifo->tail = (kvs_fifo->tail + 1) %
                   kvs_fifo->capacity;  // update tail index to point to next
                                        // entry in circular cache arr
  kvs_fifo->size++;  // increase size of cache because we added new entry

  return 0;  // return success without calling kvs_base_set when updating to
             // disk
}

int kvs_fifo_get(
    kvs_fifo_t* kvs_fifo, const char* key,
    char* value) {  // fn to get value associated with a key from FIFO cache
  if (kvs_fifo->capacity == 0)
    return kvs_base_get(
        kvs_fifo->kvs_base, key,
        value);  // case for capacity 0 set val in disk and return result
  // proceed below to look in cache ------

  for (int i = 0; i < kvs_fifo->size; ++i) {  // iterate through cache
    int index =
        (kvs_fifo->head + i) %
        kvs_fifo->capacity;  // calculate index based on head and current loop
                             // counter wrapped around cache capacity
    if (strcmp(kvs_fifo->keys[index], key) == 0) {  // if key is found in cache
      strcpy(value, kvs_fifo->values[index]);  // copy value to provided pointer
      return 0;                                // return success
    }
  }  // proceed below if the key was not found in the cache ------

  int rc = kvs_base_get(kvs_fifo->kvs_base, key,
                        value);  // attempt to retrieve from disk

  if (rc == 0) {  // if return code indicate successful retrieval from disk, add
                  // to cache
    if (kvs_fifo->size ==
        kvs_fifo->capacity) {  // if key found in disk, add to cache
      free(kvs_fifo->keys[kvs_fifo->head]);    // free key from evicted entry
      free(kvs_fifo->values[kvs_fifo->head]);  // free val from evicted entry
      kvs_fifo->head = (kvs_fifo->head + 1) %
                       kvs_fifo->capacity;  // update head index to point to
                                            // next entry in circular cache arr
      kvs_fifo->size--;  // decrease size of cache because we evicted an entry
    }

    kvs_fifo->keys[kvs_fifo->tail] =
        strdup(key);  // add new key-value pair that was fetched from disk to
                      // the tail of cache arr
    kvs_fifo->values[kvs_fifo->tail] = strdup(value);
    kvs_fifo->tail = (kvs_fifo->tail + 1) %
                     kvs_fifo->capacity;  // update tail index to point to  next
                                          // entry in circular cache arr
    kvs_fifo->size++;  // increase size of cache because we added an entry
  }

  return rc;  // return return-code of kvs_base_get
}

int kvs_fifo_flush(
    kvs_fifo_t* kvs_fifo) {  // fn to flush FIFO cache by persisting all dirty
                             // entries to disk and evicting all entries
  int rc = 0;                // initialize return code to success
  while (kvs_fifo->size > 0) {  // while entries in circular cache exist
    if (kvs_fifo->dirty[kvs_fifo->head]) {  // if entry at head is dirty
      rc = kvs_base_set(
          kvs_fifo->kvs_base, kvs_fifo->keys[kvs_fifo->head],
          kvs_fifo->values[kvs_fifo->head]);  // persist to disk and save return
                                              // code
      if (rc != 0)
        return rc;  // if kvs_base_set return code indicate failure, then return
                    // error code
    }
    free(kvs_fifo->keys[kvs_fifo->head]);    // free key of head entry
    free(kvs_fifo->values[kvs_fifo->head]);  // free val of head entry
    kvs_fifo->head = (kvs_fifo->head + 1) %
                     kvs_fifo->capacity;  // update head index to point to next
                                          // entry in circular cache arr
    kvs_fifo->size--;  // decrease size of cache because we evicted an entry
  }
  return 0;  // return success
}
