/* Logged from skigboi */
// Tests for cleaners entering and not having to wait indefinitely if students arrive after (extra credit)
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

  // student 1 comes in, can enter
  student_enter(&student1);
  // student 2 enters
  student_enter(&student2);

  // cleaning cannot enter because of students 1 and 2; this blocks until the dining hall is empty
  pthread_create(&cleaning.thread, NULL, cleaning_enter, &cleaning);
  msleep(100);

  // student 1 leaves
  student_leave(&student1);
  // student 2 leaves
  student_leave(&student2);
  // cleaner should now enter before student 3 since dining hall is empty

  pthread_create(&student3.thread, NULL, student_enter, &student3); // this should block until cleaner leaves

  // cleaning should begin now and then cleaner should leave
  pthread_join(cleaning.thread, NULL);
  msleep(100);
  cleaning_leave(&cleaning); // cleaning finishes

  // Now student 3 can enter and leave
  pthread_join(student3.thread, NULL);
  msleep(100);
  student_leave(&student3);

  dining_destroy(&d);
}
