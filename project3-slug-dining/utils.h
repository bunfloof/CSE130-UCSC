// USED FOR TESTING. DO NOT MODIFY THIS FILE.

#pragma once

#include <pthread.h>

#include "dining.h"

typedef struct student {
  dining_t* dining;
  int student_id;
  pthread_t thread;
} student_t;

student_t make_student(int student_id, dining_t* dining);

void* student_enter(void* ptr);
void* student_leave(void* ptr);

typedef struct cleaning {
  dining_t* dining;
  int cleaning_id;
  pthread_t thread;
} cleaning_t;

cleaning_t make_cleaning(int cleaning_id, dining_t* dining);

void* cleaning_enter(void* ptr);
void* cleaning_leave(void* ptr);

int msleep(long ms);
