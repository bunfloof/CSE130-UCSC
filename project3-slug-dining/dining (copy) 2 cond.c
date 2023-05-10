// clang-format off
/*********************************************************************************
 * 
 * 2023 Spring CSE130 project3
 * dining.c
 * dining reception library
 *
 * Notes:
 * (Intentionally left blank)
 *
 * Usage:
 * See comments above functions.
 *
 * Citations:
 * (n
 *
 *********************************************************************************/
#include "dining.h"

#include <pthread.h>
#include <stdlib.h>

// Class Constructors ------------------------------------------------------

typedef struct dining {
  // TODO: Add your variables here
  int capacity;
  int current_students;
  int cleaning_waiting;
  pthread_mutex_t lock;  // mutex to protect shared variables
  pthread_cond_t student_allowed;  // condition var to signal when students can enter
  pthread_cond_t cleaning_allowed;  // condition var to signal when cleaning can enter
} dining_t;

// Initialization procedures -----------------------------------------------

// dining_t* dining_init(int capacity) creates dining_t that represents the state of a dining hall. It takes an integer capacity that determines how many students can enter the dining hall at a time. A student cannot enter the dining hall if there are capacity students inside. Assume that capacity is nonnegative (capacity >= 0).
dining_t *dining_init(int capacity) {
  // TODO: Initialize necessary variables
  dining_t *dining = malloc(sizeof(dining_t));
  dining->capacity = capacity;
  dining->current_students = 0;
  dining->cleaning_waiting = 0;
  pthread_mutex_init(&dining->lock, NULL);  // initialize mutex
  pthread_cond_init(&dining->student_allowed, NULL);  // initialize student_allowed cond var
  pthread_cond_init(&dining->cleaning_allowed, NULL);  // initialize cleaning_allowed cond var
  return dining;
}

// void dining_destroy(dining_t** dining_ptr) called to destroy dining hall. Assume that this function is called when there is no student and no ongoing cleaning.
void dining_destroy(dining_t **dining) {
  // TODO: Free dynamically allocated memory
  pthread_mutex_destroy(&(*dining)->lock);  // destroy mutex
  pthread_cond_destroy(&(*dining)->student_allowed);  // destroy student_allowed cond var
  pthread_cond_destroy(&(*dining)->cleaning_allowed);  // destroy cleaning_allowed cond var
  free(*dining); // clean up
  *dining = NULL;
}

// void dining_student_enter(dining_t* dining) called when a student at the reception wants to enter dining hall. If there is a room in dining hall, this function returns. If students cannot enter dining hall, this function blocks until it becomes possible to enter.
void dining_student_enter(dining_t *dining) {
  // TODO: Your code goes here
  pthread_mutex_lock(&dining->lock);  // lock mutex to protect shared vars
  // ❗ busy waiting?
  while (dining->current_students >= dining->capacity || dining->cleaning_waiting > 0) { // wait until there's room in dining hall and no cleaning is waiting
    pthread_cond_wait(&dining->student_allowed, &dining->lock); // wait on student_allowed cond var and release mutex; once signaled, mutex will be re-acquired before continuing
  }
  dining->current_students++;  // increment current_students counter
  pthread_mutex_unlock(&dining->lock);  // unlock mutex
}

// void dining_student_leave(dining_t* dining) called when a student leaves dining hall.
void dining_student_leave(dining_t *dining) {
  // TODO: Your code goes here
  pthread_mutex_lock(&dining->lock); // lock mutex to protect shared vars
  dining->current_students--;  // decrement current_students counter
  // signal cleaning service if no students in dining hall and cleaning is waiting
  if (dining->current_students == 0 && dining->cleaning_waiting > 0) pthread_cond_signal(&dining->cleaning_allowed); // signal cleaning_allowed cond var to unblock cleaning service
  else pthread_cond_broadcast(&dining->student_allowed); // else broadcast to all waiting threads on student_allowed cond var to unblock all student threads waiting to enter dining hall
  pthread_mutex_unlock(&dining->lock); // unlock mutex
}

// void dining_cleaning_enter(dining_t* dining) called when cleaning service provider comes in to clean dining hall. The function blocks until all students leave. Once cleaning has begun, students cannot enter dining hall. Only one cleaning service provider can work in dining hall at a time.
void dining_cleaning_enter(dining_t *dining) {
  // TODO: Your code goes here
  pthread_mutex_lock(&dining->lock); // lock the mutex to protect shared vars
  dining->cleaning_waiting++;  // extra credit addition: increment cleaning_waiting counter
  // ❗ busy waiting?
  while (dining->current_students > 0) {  // wait until no students are in dining hall
    pthread_cond_wait(&dining->cleaning_allowed, &dining->lock); // wait on cleaning_allowed cond var and release mutex; once signaled, mutex will be re-acquired before continuing
  }
}

// void dining_cleaning_leave(dining_t* dining) called when cleaning is complete.
void dining_cleaning_leave(dining_t *dining) {
  // TODO: Your code goes here
  dining->cleaning_waiting--;  // extra credit addition: decrement cleaning_waiting counter
  pthread_cond_broadcast(&dining->student_allowed);  // extra credit addition: signal students that they can enter by broadcasting to all waiting threads on student_allowed cond var to unblock all student threads waiting to enter dining hall
  pthread_mutex_unlock(&dining->lock); // unlock mutex
}

/* Argument (ZH-CN to EN-US): The while loop is used in conjunction with the pthread_cond_wait() function. When the condition in the while loop is not met, the thread calls pthread_cond_wait() and goes into a waiting state. This releases the mutex and allows other threads to access the shared data. When the condition is met and thread is signaled, it re-acquires the mutex and proceeds with its work. */
