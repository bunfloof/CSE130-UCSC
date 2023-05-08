/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project2
 * mr.c
 * MapReduce-style multi-threaded data processing library
 *
 * Notes:
 * - Using examples from Rust Programming Cookbook as starter code.
 * - Thread Safety uses pthread_mutex_t to ensure thread safety when writing to
 *the output lists in both the mapper and reducer threads. Locks initialized in
 *mapper_args_t and reducer_args_t structs, and are locked/unlocked using
 *pthread_mutex_lock and pthread_mutex_unlock in the mapper and reducer threads.
 *
 * Usage:
 * This map_reduce function in this library has the following signature: void
 *map_reduce(mapper_t mapper, size_t num_mapper, reducer_t reducer, size_t
 *num_reducer, kvlist_t *input, kvlist_t *output)
 *
 * Citations:
 * (1) Matzinger, Claus. "4. Fearless Concurrency." Rust Programming Cookbook:
 *     Explore the Latest Features of Rust 2018 for Building Fast and Secure
 *          Apps. Packt Publishing, Limited. Print.
 * (2) man -k pthread_mutex_lock
 *
 *********************************************************************************/
#include "mr.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "kvlist.h"

// Class Constructors & Destructors ----------------------------------------

typedef struct {  // struct to hold the arguments for a mapper thread
  mapper_t mapper;
  kvlist_t *input;
  kvlist_t *output;
  size_t start;  // Phase 1 (Split) implicit phase handled by dividing the input
                 // between the mapper threads. Each mapper thread is assigned a
                 // range of input data by setting start and end values in
                 // mapper_args_t struct calculated based on total number of
                 // input key-value pairs (input_size) and number of mappers
                 // (num_mapper).
  size_t end;
  pthread_mutex_t *mutex;
} mapper_args_t;

typedef struct {  // struct to hold the arguments for a reducer thread
  reducer_t reducer;
  kvlist_t **partitions;
  kvlist_t *output;
  size_t id;
  size_t num_reducer;
  pthread_mutex_t *mutex;
} reducer_args_t;

// Initialization procedures -----------------------------------------------

void *mapper_thread(
    void *args) {  // mapper_thread function processes input data using provided
                   // mapper function and stores results in output kvlist
  mapper_args_t *ma = (mapper_args_t *)args;
  kvlist_iterator_t *iter =
      kvlist_iterator_new(ma->input);  // create an iterator for input list

  size_t index = 0;
  kvpair_t *kv;
  while (
      (kv = kvlist_iterator_next(iter))) {  // iterate through input list within
                                            // assigned range (start to end)
    if (index >= ma->start && index < ma->end) {
      pthread_mutex_lock(ma->mutex);
      ma->mapper(kv, ma->output);  // all mapper function for each key-value
                                   // pair within range
      pthread_mutex_unlock(ma->mutex);
    }
    index++;
  }
  kvlist_iterator_free(&iter);

  return NULL;
}

void *reducer_thread(
    void *args) {  // reducer_thread function processes partitioned data using
                   // provided reducer function and stores results in output
                   // kvlist
  reducer_args_t *ra = (reducer_args_t *)args;
  kvlist_t *input = kvlist_new();

  for (size_t i = 0; i < ra->num_reducer;
       i++) {  // merge partitioned data assigned to this reducer thread
    kvlist_extend(input, ra->partitions[i]);
  }
  kvlist_sort(input);  // sort input list for easy grouping

  char *prev_key = NULL;
  kvlist_t *group = kvlist_new();
  kvlist_iterator_t *iter = kvlist_iterator_new(input);
  kvpair_t *kv;

  while ((kv = kvlist_iterator_next(iter))) {  // group key-value pairs by key
    if (prev_key == NULL || strcmp(kv->key, prev_key) != 0) {
      if (prev_key != NULL) {
        pthread_mutex_lock(ra->mutex);
        ra->reducer(prev_key, group,
                    ra->output);  // call reducer function for each group of
                                  // key-value pairs
        pthread_mutex_unlock(ra->mutex);
        kvlist_free(&group);
        group = kvlist_new();
      }
      prev_key = kv->key;
    }
    kvlist_append(group, kvpair_clone(kv));
  }
  kvlist_iterator_free(&iter);

  if (prev_key != NULL) {
    pthread_mutex_lock(ra->mutex);
    ra->reducer(prev_key, group,
                ra->output);  // final call to reducer is needed to process the
                              // last group of key-value pairs
    pthread_mutex_unlock(ra->mutex);
  }

  kvlist_free(&input);
  kvlist_free(&group);

  return NULL;
}

void map_reduce(mapper_t mapper, size_t num_mapper, reducer_t reducer,
                size_t num_reducer, kvlist_t *input, kvlist_t *output) {
  pthread_mutex_t mapper_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t reducer_mutex = PTHREAD_MUTEX_INITIALIZER;

  size_t input_size = 0;
  {  // enclosing in separate scope to ensure that the iter variable doens't
     // conflict with other other iter varables with the same name later in the
     // function
    kvlist_iterator_t *iter =
        kvlist_iterator_new(input);  // calculate the size of the input list
    while (kvlist_iterator_next(iter)) {
      ++input_size;
    }
    kvlist_iterator_free(&iter);
  }

  // initializing mapper threads, arguments, and outputs
  pthread_t mapper_threads[num_mapper];
  mapper_args_t mapper_args[num_mapper];
  kvlist_t *mapper_outputs[num_mapper];

  for (size_t i = 0; i < num_mapper;
       ++i) {  // create mapper threads and their corresponding arguments
    mapper_outputs[i] = kvlist_new();
    mapper_args[i].mapper = mapper;
    mapper_args[i].input = input;
    mapper_args[i].output = mapper_outputs[i];
    mapper_args[i].start =
        input_size * i / num_mapper;  // divide the input data among the mapper
                                      // threads, assigning each thread a range
    mapper_args[i].end = input_size * (i + 1) /
                         num_mapper;  // of data to process based on their index
                                      // (i) and the total number of mappers
    mapper_args[i].mutex = &mapper_mutex;

    pthread_create(
        &mapper_threads[i], NULL, mapper_thread,
        &mapper_args[i]);  // Phase 2 (Map) create num_mapper mapper threads.
  }

  for (size_t i = 0; i < num_mapper;
       ++i) {  // wait for all mapper threads to finish
    pthread_join(mapper_threads[i], NULL);
  }

  kvlist_t
      *partitions[num_reducer];  // initialize partitions for reducer threads
  for (size_t i = 0; i < num_reducer; ++i) {
    partitions[i] = kvlist_new();
  }

  for (size_t i = 0; i < num_mapper;
       ++i) {  // partition output of mapper threads
    kvlist_t *mapper_output = mapper_outputs[i];
    kvlist_iterator_t *iter = kvlist_iterator_new(mapper_output);
    kvpair_t *kv;

    while ((kv = kvlist_iterator_next(
                iter))) {  // Phase 3 (Shuffle) mapper_threads loop distribute
                           // mapper outputs among reducer partitions
      size_t partition_id =
          hash(kv->key) %
          num_reducer;  // determine partition ID for a key-value pair in mapper
                        // output by hashing key and taking remainder after
                        // division by the number of reducer threads
      kvlist_append(
          partitions[partition_id],
          kvpair_clone(
              kv));  // append key-value pairs to corresponding partition
    }

    kvlist_iterator_free(&iter);
    kvlist_free(&mapper_output);
  }

  // initialize the reducer threads and corresponding arguments
  pthread_t reducer_threads[num_reducer];
  reducer_args_t reducer_args[num_reducer];

  for (size_t i = 0; i < num_reducer; ++i) {
    reducer_args[i].reducer = reducer;
    reducer_args[i].partitions = partitions;
    reducer_args[i].output = output;
    reducer_args[i].id = i;
    reducer_args[i].num_reducer = num_reducer;
    reducer_args[i].mutex = &reducer_mutex;

    pthread_create(
        &reducer_threads[i], NULL, reducer_thread,
        &reducer_args[i]);  // Phase 4 (Reduce) executed by creating num_reducer
                            // reducer threads using the pthread_create
                            // function. Each thread is given a reference to the
                            // reducer function, along with the required
                            // partitions and output list.
  }

  for (size_t i = 0; i < num_reducer;
       ++i) {  // wait for all reducer threads to finish
    pthread_join(reducer_threads[i], NULL);
  }

  for (size_t i = 0; i < num_reducer; ++i) {  // clean up partitions
    kvlist_free(&partitions[i]);
  }
}

// Phase 5 (Merge) implicit because when after the reduce phase is completed,
// the final output is already stored in the output list. output list is already
// created and shared among the reducer threads because each reducer writes its
// output directly to the shared output list.
