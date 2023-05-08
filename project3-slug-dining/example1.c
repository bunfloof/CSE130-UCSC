#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"

int main(void) {
  dining_t* d = dining_init(3);

  student_t student1 = make_student(1, d);
  student_t student2 = make_student(2, d);
  student_t student3 = make_student(3, d);
  student_t student4 = make_student(4, d);

  // 3 students can enter
  student_enter(&student1);
  student_enter(&student2);
  student_enter(&student3);

  // dining is full. student 4 cannot enter.
  pthread_create(&student4.thread, NULL, student_enter, &student4);

  // 0.1 seconds later
  msleep(100);

  // student 1 leaves
  student_leave(&student1);

  // student 4 should now be able to enter
  pthread_join(student4.thread, NULL);

  // student 2 leaves
  student_leave(&student2);

  // student 3 leaves
  student_leave(&student3);

  // student 4 leaves
  student_leave(&student4);

  dining_destroy(&d);
}
