/* 

retrieved by Shade on May 13, 2023 15:31
GenID: 4d66fff7-c2d7-4c37-aa5c-ea0869ee3016 

"Your friends don't even like you because you're not white." -Akogeno

*/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"

/* This test case is designed for an extra credit scenario where a cleaner thread is spawned before some student threads. 
 * The expectation is that the cleaner should enter as soon as the first student leaves, even if other students are waiting.
 */
int main(void) {
  // Initialize the dining hall with a capacity of 3.
  dining_t* d = dining_init(3);

  // Create students and cleaner.
  student_t student1 = make_student(1, d);
  student_t student2 = make_student(2, d);
  student_t student3 = make_student(3, d);
  cleaning_t cleaning = make_cleaning(1, d);

  // Student 1 enters the dining hall. Since the hall is empty, entry should be immediate.
  pthread_create(&student1.thread, NULL, student_enter, &student1);

  msleep(100);
  // Attempt to have the cleaner enter the dining hall. This should block because Student 1 is inside.
  pthread_create(&cleaning.thread, NULL, cleaning_enter, &cleaning);

  msleep(100);
  // Student 2 attempts to enter the dining hall. This should block because the cleaner is waiting.
  pthread_create(&student2.thread, NULL, student_enter, &student2);

  msleep(100);
  // Student 3 attempts to enter the dining hall. This should also block due to the waiting cleaner.
  pthread_create(&student3.thread, NULL, student_enter, &student3);

  msleep(100);
  // Student 1 finishes their stay in the dining hall and leaves. This should allow the cleaner to enter.
  pthread_join(student1.thread, NULL);
  student_leave(&student1);

  msleep(100);
  // After the cleaner finishes, the dining hall reopens. Students 2 and 3, who have been waiting, can now enter.
  pthread_join(cleaning.thread, NULL);
  cleaning_leave(&cleaning);

  msleep(100);
  // Student 2 finishes their stay and leaves the dining hall.
  pthread_join(student2.thread, NULL);
  student_leave(&student2);

  msleep(100);
  // Student 3 also finishes their stay and leaves the dining hall.
  pthread_join(student3.thread, NULL);
  student_leave(&student3);

  // Finally, destroy the dining hall to clean up.
  dining_destroy(&d);
}
