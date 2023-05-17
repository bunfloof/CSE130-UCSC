#include "kvs_base.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

kvs_base_t* kvs_base_new(const char* directory) {
  kvs_base_t* kvs_base = malloc(sizeof(kvs_base_t));
  if (kvs_base == NULL) {
    return NULL;
  }

  // create directory if not exist
  struct stat st;
  if (stat(directory, &st) == -1) {
    int rc = mkdir(directory, S_IRWXU | S_IRWXG | S_IRWXO);
    if (rc != 0) {
      return NULL;
    }
  }
  strcpy(kvs_base->directory, directory);

  kvs_base->get_count = 0;
  kvs_base->set_count = 0;

  return kvs_base;
}

void kvs_base_free(kvs_base_t** ptr) {
  free(*ptr);
  *ptr = NULL;
}

int kvs_base_set(kvs_base_t* kvs, const char* key, const char* value) {
  int rc;
  char filename[PATH_MAX];
  strcpy(filename, kvs->directory);
  strcat(filename, "/");
  strcat(filename, key);
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    return -1;
  }
  fwrite(value, sizeof(char), strlen(value), fp);
  rc = fclose(fp);
  if (rc != 0) {
    return rc;
  }
  kvs->set_count += 1;
  return 0;
}

int kvs_base_get(kvs_base_t* kvs, const char* key, char* value) {
  int rc;
  char filename[PATH_MAX];
  strcpy(filename, kvs->directory);
  strcat(filename, "/");
  strcat(filename, key);
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    // if the file doesn't exist, return the empty string
    kvs->get_count += 1;
    strcpy(value, "");
    return 0;
  }
  size_t num_read = fread(value, sizeof(char), KVS_VALUE_MAX, fp);
  value[num_read] = '\0';
  rc = fclose(fp);
  if (rc != 0) {
    return rc;
  }
  kvs->get_count += 1;
  return 0;
}
