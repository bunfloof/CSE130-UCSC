#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"

#define NUM_STUDENTS 5
#define NUM_CLEANERS 2

typedef struct {
  int id;
  dining_t* dining_hall;
} thread_arg_t;

void *student(void *arg) {
  thread_arg_t* thread_arg = (thread_arg_t*)arg;
  int id = thread_arg->id;
  dining_t* dining_hall = thread_arg->dining_hall;
  printf("Student %d comes in\n", id);
  dining_student_enter(dining_hall);
  printf("Student %d entered\n", id);
  sleep(1);
  dining_student_leave(dining_hall);
  printf("Student %d leaves\n", id);
  return NULL;
}

void *cleaner(void *arg) {
  thread_arg_t* thread_arg = (thread_arg_t*)arg;
  int id = thread_arg->id;
  dining_t* dining_hall = thread_arg->dining_hall;
  printf("Cleaning %d comes in\n", id);
  dining_cleaning_enter(dining_hall);
  printf("Cleaning %d entered\n", id);
  sleep(1);
  dining_cleaning_leave(dining_hall);
  printf("Cleaning %d leaves\n", id);
  return NULL;
}

int main(void) {
  dining_t *d = dining_init(3);

  pthread_t students[NUM_STUDENTS];
  pthread_t cleaners[NUM_CLEANERS];
  thread_arg_t args[NUM_STUDENTS + NUM_CLEANERS];

  for (int i = 0; i < NUM_STUDENTS; i++) {
    args[i].id = i + 1;
    args[i].dining_hall = d;
    pthread_create(&students[i], NULL, student, &args[i]);
    if (i < NUM_CLEANERS) {
      pthread_create(&cleaners[i], NULL, cleaner, &args[i]);
    }
  }

  for (int i = 0; i < NUM_STUDENTS; i++) {
    pthread_join(students[i], NULL);
    if (i < NUM_CLEANERS) {
      pthread_join(cleaners[i], NULL);
    }
  }

  dining_destroy(&d);

  return 0;
}
