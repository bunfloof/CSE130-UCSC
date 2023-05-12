#include "dining.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

struct dining {
  int capacity;
  int students;
  int cleaning;
  int cleaner_waiting;
  pthread_mutex_t mtx;
  pthread_cond_t cv_students;
  pthread_cond_t cv_cleaner;
};

dining_t *dining_init(int capacity) {
  dining_t *d = malloc(sizeof(dining_t));
  d->capacity = capacity;
  d->students = 0;
  d->cleaning = 0;
  d->cleaner_waiting = 0;
  pthread_mutex_init(&d->mtx, NULL);
  pthread_cond_init(&d->cv_students, NULL);
  pthread_cond_init(&d->cv_cleaner, NULL);
  return d;
}

void dining_destroy(dining_t **d) {
  pthread_mutex_destroy(&(*d)->mtx);
  pthread_cond_destroy(&(*d)->cv_students);
  pthread_cond_destroy(&(*d)->cv_cleaner);
  free(*d);
  *d = NULL;
}

void dining_student_enter(dining_t *d) {
  pthread_mutex_lock(&d->mtx);
  while (d->students == d->capacity || d->cleaning > 0) {
    pthread_cond_wait(&d->cv_students, &d->mtx);
  }
  d->students++;
  pthread_mutex_unlock(&d->mtx);
}

void dining_student_leave(dining_t *d) {
  pthread_mutex_lock(&d->mtx);
  d->students--;
  if (d->students == 0 && d->cleaner_waiting > 0) {
    pthread_cond_signal(&d->cv_cleaner);
  } else if (d->students < d->capacity) {
    pthread_cond_signal(&d->cv_students);
  }
  pthread_mutex_unlock(&d->mtx);
}

void dining_cleaning_enter(dining_t *d) {
  pthread_mutex_lock(&d->mtx);
  d->cleaner_waiting++;
  while (d->students > 0 || d->cleaning > 0) {
    pthread_cond_wait(&d->cv_cleaner, &d->mtx);
  }
  d->cleaner_waiting--;
  d->cleaning++;
  pthread_mutex_unlock(&d->mtx);
}

void dining_cleaning_leave(dining_t *d) {
  pthread_mutex_lock(&d->mtx);
  d->cleaning--;
  if (d->cleaning == 0) {
    pthread_cond_signal(&d->cv_students);
    pthread_cond_signal(&d->cv_cleaner);  // signal other cleaners to enter
  }
  pthread_mutex_unlock(&d->mtx);
}
