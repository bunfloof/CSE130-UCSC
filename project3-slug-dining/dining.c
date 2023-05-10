#include "dining.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct dining {
  int capacity;
  int current_students;
  int cleaning_waiting;
  int current_cleaners; 
  pthread_mutex_t lock;  
  pthread_cond_t student_allowed;  
  pthread_cond_t cleaning_allowed;  
} dining_t;

dining_t *dining_init(int capacity) {
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  dining->current_students = 0;
  dining->cleaning_waiting = 0;
  pthread_mutex_init(&dining->lock, NULL);  
  pthread_cond_init(&dining->student_allowed, NULL);  
  pthread_cond_init(&dining->cleaning_allowed, NULL);  
  return dining;
}

void dining_destroy(dining_t **dining) {
  pthread_mutex_destroy(&(*dining)->lock);  
  pthread_cond_destroy(&(*dining)->student_allowed);  
  pthread_cond_destroy(&(*dining)->cleaning_allowed);  
  free(*dining); 
  *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock);  
  // ❗ busy waiting?
  while (dining->current_students >= dining->capacity || dining->cleaning_waiting > 0) { 
    pthread_cond_wait(&dining->student_allowed, &dining->lock); 
  }
  dining->current_students++;  
  pthread_mutex_unlock(&dining->lock);  
}

void dining_student_leave(dining_t *dining) {
  pthread_mutex_lock(&dining->lock); 
  dining->current_students--;  
  if (dining->current_students == 0 && dining->cleaning_waiting > 0) pthread_cond_signal(&dining->cleaning_allowed); 
  else pthread_cond_broadcast(&dining->student_allowed); 
  pthread_mutex_unlock(&dining->lock); 
}

void dining_cleaning_enter(dining_t *dining) {
  pthread_mutex_lock(&dining->lock); 
  dining->cleaning_waiting++; // 🍃 extra credit addition: increment cleaning_waiting counter
  // ❗ busy waiting?
  while (dining->current_students > 0 || dining->current_cleaners > 0) {  
    pthread_cond_wait(&dining->cleaning_allowed, &dining->lock); 
  }
  dining->current_cleaners++; 
}

void dining_cleaning_leave(dining_t *dining) {
  dining->cleaning_waiting--;  // 🍃 extra credit addition: decrement cleaning_waiting counter
  dining->current_cleaners--; 
  if (dining->cleaning_waiting > 0) pthread_cond_signal(&dining->cleaning_allowed);  // 🍃 extra credit addition: signal students that they can enter by broadcasting to all waiting threads on student_allowed cond var to unblock all student threads waiting to enter dining hall
  pthread_cond_broadcast(&dining->student_allowed);  
  pthread_mutex_unlock(&dining->lock); 
}

/*


*/

