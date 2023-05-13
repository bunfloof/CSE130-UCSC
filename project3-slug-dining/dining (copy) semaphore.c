// clang-format off
/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project3
 * dining.c
 * dining reception library
 *
 * Notes:
 * - Semaphore implementation inspired by other students
 * - Extra credit clarification: cleaning staff doesn't wait indefinitely when capacity is zero
 *
 * Usage:
 * See comments above functions.
 *
 * Checks:
 *
 * Citations:
 * - Dongjing tutor
 *
 *********************************************************************************/
#include "dining.h"
#include "utils.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct dining {
  sem_t *student_sem; // semaphore for students
  pthread_mutex_t cleaning_mtx; // mutex for cleaning staff
  int cleaning_in_progress; // flag to check if cleaning is in progress
  int capacity; // capacity of the dining hall
} dining_t;

dining_t *dining_init(int capacity) {
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  
  dining->student_sem = malloc(sizeof(sem_t)); // initialize semaphore for students with the dining hall capacity
  sem_init(dining->student_sem, 0, capacity);

  pthread_mutex_init(&dining->cleaning_mtx, NULL); // initialize mutex for cleaning staff and cleaning in progress flag
  dining->cleaning_in_progress = 0;

  return dining;
}

void dining_destroy(dining_t **dining) {
  sem_destroy((*dining)->student_sem);
  free((*dining)->student_sem);
  pthread_mutex_destroy(&(*dining)->cleaning_mtx);
  free(*dining);
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  // extra credit addition: students can enter only if cleaning is not in progress and hall is not full
  while (1) {
    pthread_mutex_lock(&dining->cleaning_mtx);
    if (!dining->cleaning_in_progress) {
      sem_wait(dining->student_sem);
      pthread_mutex_unlock(&dining->cleaning_mtx);
      break;
    }
    pthread_mutex_unlock(&dining->cleaning_mtx);
  }
}

void dining_student_leave(dining_t *dining) {
  sem_post(dining->student_sem);
}

void dining_cleaning_enter(dining_t *dining) {
  // extra credit: cleaning staff can enter only if no students are present in the dining hall
  pthread_mutex_lock(&dining->cleaning_mtx);
  if (dining->capacity > 0) {  // if capacity is 0, cleaners can enter directly
    for (int i = 0; i < dining->capacity; i++) {
      sem_wait(dining->student_sem);
    }
  }
  dining->cleaning_in_progress = 1;
  pthread_mutex_unlock(&dining->cleaning_mtx);
}

void dining_cleaning_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->cleaning_mtx);
  for (int i = 0; i < dining->capacity; i++) {
    sem_post(dining->student_sem);
  }
  dining->cleaning_in_progress = 0;
  pthread_mutex_unlock(&dining->cleaning_mtx);
}
