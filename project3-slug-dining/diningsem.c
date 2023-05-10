#include "dining.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct dining {
  int capacity;
  int current_students;
  int cleaning_waiting;
  sem_t student_sem;
  sem_t cleaning_sem;
  pthread_mutex_t student_mutex;
} dining_t;

dining_t *dining_init(int capacity) {
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  dining->current_students = 0;
  dining->cleaning_waiting = 0;
  sem_init(&dining->student_sem, 0, capacity);
  sem_init(&dining->cleaning_sem, 0, 0);
  pthread_mutex_init(&dining->student_mutex, NULL);
  return dining;
}

void dining_destroy(dining_t **dining) {
  sem_destroy(&(*dining)->student_sem);
  sem_destroy(&(*dining)->cleaning_sem);
  pthread_mutex_destroy(&(*dining)->student_mutex);
  free(*dining);
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->student_mutex);
  if (dining->cleaning_waiting > 0) {
    pthread_mutex_unlock(&dining->student_mutex);
    sem_wait(&dining->student_sem);
    pthread_mutex_lock(&dining->student_mutex);
  } else {
    sem_wait(&dining->student_sem);
  }
  dining->current_students++;
  pthread_mutex_unlock(&dining->student_mutex);
}

void dining_student_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->student_mutex);
  dining->current_students--;
  if (dining->current_students == 0 && dining->cleaning_waiting > 0) {
    sem_post(&dining->cleaning_sem);
  } else {
    sem_post(&dining->student_sem);
  }
  pthread_mutex_unlock(&dining->student_mutex);
}

void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->student_mutex);
  dining->cleaning_waiting++;
  if (dining->current_students > 0) {
    pthread_mutex_unlock(&dining->student_mutex);
    sem_wait(&dining->cleaning_sem);
  } else {
    pthread_mutex_unlock(&dining->student_mutex);
  }
}

void dining_cleaning_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->student_mutex);
  dining->cleaning_waiting--;
  sem_post(&dining->student_sem);
  pthread_mutex_unlock(&dining->student_mutex);
}
