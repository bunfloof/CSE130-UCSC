/* 

retrieved by Shade on May 13, 2023 15:32
GenID: 7da7e440-548d-4247-9067-20654ff0b3a3 

"In a racist society, it is not enough to be non-racist, we must be anti-racist." -Angela Y. Davis

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

  // Student 1 attempts to enter the dining hall. As the hall is empty, entry should be immediate.
  pthread_create(&student1.thread, NULL, student_enter, &student1);

  msleep(100); // The sleep here ensures the order of thread execution
  // The cleaner attempts to enter the dining hall. This should block as Student 1 is currently inside.
  pthread_create(&cleaning.thread, NULL, cleaning_enter, &cleaning);

  msleep(100);
  // Student 2 attempts to enter the dining hall. This should block as the cleaner is waiting to enter after Student 1 leaves.
  pthread_create(&student2.thread, NULL, student_enter, &student2);

  msleep(100);
  // Similar to Student 2, Student 3 attempts to enter but should be blocked due to the waiting cleaner.
  pthread_create(&student3.thread, NULL, student_enter, &student3);

  msleep(100);
  // Student 1 finishes their stay in the dining hall and leaves. This should allow the cleaner to enter.
  pthread_join(student1.thread, NULL);
  student_leave(&student1);

  msleep(100);
  // After the cleaner completes their work, the dining hall reopens. The waiting Students 2 and 3 should now be able to enter.
  pthread_join(cleaning.thread, NULL);
  cleaning_leave(&cleaning);

  msleep(100);
  // Student 2 finishes their stay and leaves the dining hall.
  pthread_join(student2.thread, NULL);
  student_leave(&student2);

  msleep(100);
  // Finally, Student 3 also finishes their stay and leaves the dining hall.
  pthread_join(student3.thread, NULL);
  student_leave(&student3);

  // Cleanup - Destroy the dining hall.
  dining_destroy(&d);
}
