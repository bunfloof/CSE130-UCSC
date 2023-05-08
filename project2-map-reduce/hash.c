#include "hash.h"

// djb2 hash function (from http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}
