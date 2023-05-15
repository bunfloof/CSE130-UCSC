#include <pthread.h>
#include <stdlib.h>

#include "dining.h"

typedef struct dining {
  int capacity;
  int current_students;
  int current_cleaners;  // New variable to keep track of the number of cleaners
                         // currently in the dining hall
  int cleaning_waiting;
  pthread_mutex_t lock;
  pthread_cond_t student_allowed;
  pthread_cond_t cleaning_allowed;
} dining_t;

void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  dining->cleaning_waiting++;
  // wait until no students are in the dining hall and no other cleaner is
  // working
  while (dining->current_students > 0 || dining->current_cleaners > 0) {
    pthread_cond_wait(&dining->cleaning_allowed, &dining->lock);
  }
  dining->current_cleaners++;  // Increment the number of cleaners currently in
                               // the dining hall
}

void dining_cleaning_leave(dining_t *dining) {
  dining->cleaning_waiting--;
  dining->current_cleaners--;  // Decrement the number of cleaners currently in
                               // the dining hall
  if (dining->cleaning_waiting > 0) {
    pthread_cond_signal(
        &dining->cleaning_allowed);  // Signal the next cleaner to start if any
                                     // are waiting
  }
  pthread_cond_broadcast(
      &dining->student_allowed);  // Signal students that they can enter
  pthread_mutex_unlock(&dining->lock);
}

// fix not working

void dining_student_leave(dining_t *dining) {

  pthread_mutex_lock(&dining->lock); 
  dining->current_students--;  

  if (dining->current_students == 0 && dining->cleaning_waiting > 0) {
    pthread_cond_signal(&dining->cleaning_allowed); 
  } else if (dining->current_students == 0 && dining->cleaning_waiting == 0) {
    pthread_cond_broadcast(&dining->student_allowed); 
  }

  pthread_mutex_unlock(&dining->lock); 
}

void dining_cleaning_leave(dining_t *dining) {

  pthread_mutex_lock(&dining->lock); 
  dining->cleaning_waiting--;  
  dining->current_cleaners--; 

  if (dining->cleaning_waiting > 0) {
    pthread_cond_signal(&dining->cleaning_allowed); 
  } else if (dining->cleaning_waiting == 0) {
    pthread_cond_broadcast(&dining->student_allowed);  
  }

  pthread_mutex_unlock(&dining->lock); 
}

/*
- Cleaning 1 enters and starts cleaning.
- Student 1 arrives and waits because cleaning is in progress.
- Cleaning 2 arrives and waits because cleaning 1 is in progress.
- Student 2 arrives and also waits.
- Cleaning 1 leaves and signals for the next cleaning, which is Cleaning 2.
- However, Cleaning 2 doesn't enter because Student 1 is now in the dining hall. <--- deadlock
*/

// the fix below fixes ./examplediscord5 but breaks ./examplediscord3:

void dining_student_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  while (dining->current_students >= dining->capacity || dining->cleaning_waiting > 0 || dining->current_cleaners > 0) {
    pthread_cond_wait(&dining->student_allowed, &dining->lock);
  }
  dining->current_students++;
  pthread_mutex_unlock(&dining->lock);
}

void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  dining->cleaning_waiting++;
  while (dining->current_students > 0 || dining->current_cleaners > 0) {
    pthread_cond_wait(&dining->cleaning_allowed, &dining->lock);
  }
  dining->cleaning_waiting--;
  dining->current_cleaners++;
  pthread_mutex_unlock(&dining->lock);
}

void dining_student_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  dining->current_students--;
  if (dining->current_students == 0 && (dining->cleaning_waiting > 0 || dining->current_cleaners > 0)) {
    pthread_cond_broadcast(&dining->cleaning_allowed);
  } else {
    pthread_cond_broadcast(&dining->student_allowed);
  }
  pthread_mutex_unlock(&dining->lock);
}

void dining_cleaning_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);
  dining->current_cleaners--;
  if (dining->current_cleaners == 0 && dining->cleaning_waiting > 0) {
    pthread_cond_broadcast(&dining->cleaning_allowed);
  } else {
    pthread_cond_broadcast(&dining->student_allowed);
  }
  pthread_mutex_unlock(&dining->lock);
}