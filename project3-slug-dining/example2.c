#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"

int main(void) {
  dining_t* d = dining_init(3);

  student_t student1 = make_student(1, d);
  student_t student2 = make_student(2, d);
  cleaning_t cleaning = make_cleaning(1, d);

  // student 1 comes in, can enter
  student_enter(&student1);

  // cleaning cannot enter because of student 1; this blocks
  pthread_create(&cleaning.thread, NULL, cleaning_enter, &cleaning);
  msleep(100);

  // student 1 leaves
  student_leave(&student1);

  // cleaning should begin now
  pthread_join(cleaning.thread, NULL);

  // student 2 comes in but cannot enter because the cleaning is in progress
  pthread_create(&student2.thread, NULL, student_enter, &student2);

  // 0.1 seconds later
  msleep(100);

  // cleaning completes
  cleaning_leave(&cleaning);

  // now, student 2 should be able to enter
  pthread_join(student2.thread, NULL);
  student_leave(&student2);

  dining_destroy(&d);
}
