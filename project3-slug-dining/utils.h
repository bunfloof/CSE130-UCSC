/* Retrieved from NGX on May 5, 2023 MODIFIED May 12 17:30 
(This is not the original utils.h file; please revert back on final submission.) */
// USED FOR TESTING. DO NOT MODIFY THIS FILE.

#pragma once

#include <pthread.h>
#include <stdbool.h>

#include "dining.h"

typedef struct student {
  dining_t* dining;
  int student_id;
  pthread_t thread;
  bool entered;
} student_t;

student_t make_student(int student_id, dining_t* dining);

void* student_enter(void* ptr);
void* student_leave(void* ptr);

typedef struct cleaning {
  dining_t* dining;
  int cleaning_id;
  pthread_t thread;
  bool entered;
} cleaning_t;

cleaning_t make_cleaning(int cleaning_id, dining_t* dining);

void* cleaning_enter(void* ptr);
void* cleaning_leave(void* ptr);

int msleep(long ms);
