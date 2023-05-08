#include <pthread.h>
#include <stdlib.h>

#include "dining.h"

typedef struct dining {
  // TODO: Add your variables here
  int capacity;
} dining_t;

dining_t *dining_init(int capacity) {
  // TODO: Initialize necessary variables
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  return dining;
}
void dining_destroy(dining_t **dining) {
  // TODO: Free dynamically allocated memory
  free(*dining);
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  // TODO: Your code goes here
}

void dining_student_leave(dining_t *dining) {
  // TODO: Your code goes here
}

void dining_cleaning_enter(dining_t *dining) {
  // TODO: Your code goes here
}

void dining_cleaning_leave(dining_t *dining) {
  // TODO: Your code goes here
}
