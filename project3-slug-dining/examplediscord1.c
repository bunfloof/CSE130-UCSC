/* Logged by from super */
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
  cleaning_t cleaning = make_cleaning(1, d);
  //cleaning_t cleaning2 = make_cleaning(2, d);
  //Student 1 Enters the Dining Hall
  pthread_create(&student1.thread, NULL, student_enter, &student1);

  msleep(100);
  //The cleaner enters. He cannot clean since Student 1 is currently eating.
  pthread_create(&cleaning.thread, NULL, cleaning_enter, &cleaning);

  msleep(100);
  //Now Student 2 Enters and begins waiting in line. He should not be able to enter since the Cleaner is waiting
  pthread_create(&student2.thread, NULL, student_enter, &student2);

  msleep(100);
  //Similar to Student 2, this student joins the line but cannot enter due to the cleaner waiting.
  pthread_create(&student3.thread, NULL, student_enter, &student3);
  
  msleep(100);
  //Now Student 1 finishes eating and leaves the dining hall. Now the cleaner can begin without having to wait for other students.
  pthread_join(student1.thread, NULL);
  student_leave(&student1);

  msleep(100);
  //Once the cleaner finishes, the dining hall opens up to other students that were waiting. Now Students 2 and 3 begin eating.
  pthread_join(cleaning.thread, NULL);
  cleaning_leave(&cleaning);

  msleep(100);
  //Student 2 leaves
  pthread_join(student2.thread, NULL);
  student_leave(&student2);

  msleep(100);
  //Student 3 leaves
  pthread_join(student3.thread, NULL);
  student_leave(&student3);

  dining_destroy(&d);
}
