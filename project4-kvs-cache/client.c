#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kvs.h"

/**
 * `get_replacement_policy` takes a string representation of the cache
 * replacement policy and returns the value of enum `kvs_replacement_policy`. If
 * the input is unknown, it falls back to the NONE policy and prints a warning
 * message.
 */
kvs_replacement_policy get_replacement_policy(const char* policy) {
  if (strcmp(policy, "NONE") == 0) {
    return KVS_CACHE_NONE;
  }
  if (strcmp(policy, "FIFO") == 0) {
    return KVS_CACHE_FIFO;
  }
  if (strcmp(policy, "CLOCK") == 0) {
    return KVS_CACHE_CLOCK;
  }
  if (strcmp(policy, "LRU") == 0) {
    return KVS_CACHE_LRU;
  }
  warnx("invalid cache replacement policy %s: falling back to NONE", policy);
  return KVS_CACHE_NONE;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s DIRECTORY POLICY CAPACITY\n", argv[0]);
    return 1;
  }
  int rc;
  char line[KVS_KEY_MAX + KVS_VALUE_MAX + 128];
  char value[KVS_VALUE_MAX];

  const char* directory = argv[1];
  kvs_replacement_policy replacement_policy = get_replacement_policy(argv[2]);
  int capacity = atoi(argv[3]);

  kvs_t* kvs = kvs_new(directory, replacement_policy, capacity);
  if (kvs == NULL) {
    fprintf(stderr, "kvs_new failed\n");
    return 1;
  }

  while (fgets(line, sizeof(line), stdin) != NULL) {
    // if the line ends with a newline, remove it
    int len = strlen(line);
    if (line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }
    if (strncmp(line, "GET ", 4) == 0 && line[4] != '\0') {
      rc = kvs_get(kvs, line + 4, value);
      if (rc != 0) {
        fprintf(stderr, "GET ERROR\n");
        return 1;
      }
      printf("%s\n", value);
      continue;
    }
    if (strncmp(line, "SET ", 4) == 0 && line[4] != '\0') {
      int key_length = 0;
      while (line[4 + key_length] != ' ' && line[4 + key_length] != '\0') {
        key_length += 1;
      }
      line[4 + key_length] = '\0';
      rc = kvs_set(kvs, line + 4, line + 4 + key_length + 1);
      if (rc != 0) {
        fprintf(stderr, "SET ERROR\n");
        return 1;
      }
      continue;
    }
  }

  kvs_flush(kvs);

  printf("GET COUNT (CACHE): %d\n", kvs->get_count);
  printf("GET COUNT (DISK): %d\n", kvs->kvs_base->get_count);
  printf("GET CACHE HIT RATE: %.2f%%\n",
         (kvs->get_count - kvs->kvs_base->get_count + 0.00) / kvs->get_count *
             100);
  printf("SET COUNT (CACHE): %d\n", kvs->set_count);
  printf("SET COUNT (DISK): %d\n", kvs->kvs_base->set_count);
  printf("SET CACHE HIT RATE: %.2f%%\n",
         (kvs->set_count - kvs->kvs_base->set_count + 0.00) / kvs->set_count *
             100);

  kvs_free(&kvs);
}
