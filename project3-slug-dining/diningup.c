#include "dining.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct dining {
  int capacity;
  int current_students;
  int current_cleaners; // New variable to keep track of the number of cleaners currently in the dining hall
  int cleaning_waiting;
  pthread_mutex_t lock;
  pthread_cond_t student_allowed;
  pthread_cond_t cleaning_allowed;
} dining_t;


void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  dining->cleaning_waiting++;
  // wait until no students are in the dining hall and no other cleaner is working
  while (dining->current_students > 0 || dining->current_cleaners > 0) {
    pthread_cond_wait(&dining->cleaning_allowed, &dining->lock);
  }
  dining->current_cleaners++; // Increment the number of cleaners currently in the dining hall
}

void dining_cleaning_leave(dining_t *dining) {
  dining->cleaning_waiting--;
  dining->current_cleaners--; // Decrement the number of cleaners currently in the dining hall
  if (dining->cleaning_waiting > 0) {
    pthread_cond_signal(&dining->cleaning_allowed); // Signal the next cleaner to start if any are waiting
  }
  pthread_cond_broadcast(&dining->student_allowed); // Signal students that they can enter
  pthread_mutex_unlock(&dining->lock);
}
