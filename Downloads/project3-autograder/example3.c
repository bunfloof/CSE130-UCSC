#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"

int main(void) {
  dining_t* d = dining_init(3);

  student_t student1 = make_student(1, d);
  student_t student2 = make_student(2, d);
  cleaning_t cleaning1 = make_cleaning(1, d);
  cleaning_t cleaning2 = make_cleaning(2, d);

  // student 1 comes in, can enter
  student_enter(&student1);

  // cleaning cannot enter because of student 1; this blocks
  pthread_create(&cleaning1.thread, NULL, cleaning_enter, &cleaning1);

  // this let cleaning to begin
  student_leave(&student1);

  pthread_join(cleaning1.thread, NULL);

  // cleaning in progress, this also blocks
  pthread_create(&student2.thread, NULL, student_enter, &student2);

  // another cleaning service comes in
  pthread_create(&cleaning2.thread, NULL, cleaning_enter, &cleaning2);

  sleep(1);

  cleaning_leave(&cleaning1);

  sleep(1);

  assert(cleaning2.entered || student2.entered);

  if (cleaning2.entered) {
    pthread_join(cleaning2.thread, NULL);
    cleaning_leave(&cleaning2);
    pthread_join(student2.thread, NULL);
    student_leave(&student2);
  } else {
    pthread_join(student2.thread, NULL);
    student_leave(&student2);
    pthread_join(cleaning2.thread, NULL);
    cleaning_leave(&cleaning2);
  }
  dining_destroy(&d);
}
