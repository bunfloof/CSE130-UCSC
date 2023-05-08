#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kvlist.h"
#include "mr.h"

/**
 * `toLowerStr` is a helper function that turns the string lowercased
 */
char *toLowerStr(char *s) {
  for (char *p = s; *p; p++) {
    *p = tolower(*p);
  }
  return s;
}

/**
 * `mapper` is a function to be passed to `map_reduce`
 *
 * `pair->value` has one line of the text.
 * `mapper` needs to read the line into words and and
 * add ($WORD, "1") to the `output` list.
 */
void mapper(kvpair_t *pair, kvlist_t *output) {
  char delim[] = "\t\r\n !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
  char *token;
  char *saveptr;

  // use `strtok_r` to split the line into words and add pairs to `output`
  token = strtok_r(pair->value, delim, &saveptr);
  if (token == NULL) {
    return;
  }
  kvlist_append(output, kvpair_new(toLowerStr(token), "1"));
  while ((token = strtok_r(NULL, delim, &saveptr)) != NULL) {
    kvlist_append(output, kvpair_new(toLowerStr(token), "1"));
  }
}

/**
 * `reducer` is a function to be passed to `map_reduce`
 *
 * `key` contains the string key to be aggregated.
 * `lst` is a list of pairs. All keys in this list is the same as `key`.
 *
 * `reducer` adds up the numbers in the `value` field of the pairs and
 *
 */
void reducer(char *key, kvlist_t *lst, kvlist_t *output) {
  int sum = 0;

  // iterate through lst
  kvlist_iterator_t *itor = kvlist_iterator_new(lst);
  for (;;) {
    kvpair_t *pair = kvlist_iterator_next(itor);
    if (pair == NULL) {
      break;
    }
    // turn the value into an integer and increment the sum
    sum += atoi(pair->value);
  }
  kvlist_iterator_free(&itor);

  // turn `sum` back to a string and add to output
  char sum_string[32];
  sprintf(sum_string, "%d", sum);
  kvlist_append(output, kvpair_new(key, sum_string));
}

int main(int argc, char **argv) {
  // parse arguments
  if (argc < 4) {
    fprintf(stderr, "Usage: %s num_mapper num_reducer file...\n", argv[0]);
    return 1;
  }
  int num_mapper = strtol(argv[1], NULL, 10);
  int num_reducer = strtol(argv[2], NULL, 10);
  if (num_mapper <= 0 || num_reducer <= 0) {
    fprintf(stderr, "num_mapper and num_reducer must be positive\n");
    return 1;
  }

  // input is a list of words
  kvlist_t *input = kvlist_new();

  // for each file
  for (int i = 3; i < argc; i++) {
    char *filename = argv[i];
    // open the file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      kvlist_free(&input);
      err(1, "%s", filename);
    }
    // variables required for reading the file by line
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    // read the file by line
    while ((read = getline(&line, &len, fp)) != -1) {
      kvlist_append(input, kvpair_new(filename, line));
    }
    // cleanup
    fclose(fp);
    if (line) {
      free(line);
    }
    line = NULL;
  }

  // create output list
  kvlist_t *output = kvlist_new();

  // call map_reduce
  map_reduce(mapper, num_mapper, reducer, num_reducer, input, output);

  // print out the result
  kvlist_print(1, output);

  // cleanup
  kvlist_free(&input);
  kvlist_free(&output);
  return 0;
}
