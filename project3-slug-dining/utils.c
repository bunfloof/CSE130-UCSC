// USED FOR TESTING. DO NOT MODIFY THIS FILE.
#define _POSIX_C_SOURCE 200809L

#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

student_t make_student(int student_id, dining_t* dining) {
  student_t s;
  s.student_id = student_id;
  s.dining = dining;
  return s;
}

void* student_enter(void* ptr) {
  student_t* s = (student_t*)ptr;
  printf("Student %d comes in\n", s->student_id);
  dining_student_enter(s->dining);
  printf("Student %d entered\n", s->student_id);
  return NULL;
}

void* student_leave(void* ptr) {
  student_t* s = (student_t*)ptr;
  printf("Student %d leaves\n", s->student_id);
  dining_student_leave(s->dining);
  printf("Student %d left\n", s->student_id);
  return NULL;
}

cleaning_t make_cleaning(int cleaning_id, dining_t* dining) {
  cleaning_t c;
  c.cleaning_id = cleaning_id;
  c.dining = dining;
  return c;
}

void* cleaning_enter(void* ptr) {
  cleaning_t* c = (cleaning_t*)ptr;
  printf("Cleaning %d comes in\n", c->cleaning_id);
  dining_cleaning_enter(c->dining);
  printf("Cleaning %d entered\n", c->cleaning_id);
  return NULL;
}

void* cleaning_leave(void* ptr) {
  cleaning_t* c = (cleaning_t*)ptr;
  printf("Cleaning %d leaves\n", c->cleaning_id);
  dining_cleaning_leave(c->dining);
  printf("Cleaning %d left\n", c->cleaning_id);
  return NULL;
}

int msleep(long ms) {
  struct timespec ts;
  int res;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);
  return res;
}
