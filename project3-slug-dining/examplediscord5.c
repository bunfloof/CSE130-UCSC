/* Logged from ezekiel
* 
* In Super's case, the cleaner enters first, then a student enters, then another cleaner enters, and finally another 
* student enters. Then it's verified that the cleaners and students leave in the expected order, given the constraints 
* of the dining hall.
* 
* In Ezekiel's case, the sequence of events is the same up to the point where the second student enters. However,
* after this, the two cleaners leave before the students do. This is a different scenario, and one that could 
* cause a deadlock if the cleaners are waiting for the students to leave, but the students are waiting for the 
* cleaners to leave.
*
*/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"
// Tests for cleaners entering and not having to wait indefinitely if students
// arrive after (extra credit)
int main(void) {
  dining_t* d = dining_init(3);

  student_t student1 = make_student(1, d);
  student_t student2 = make_student(2, d);
  cleaning_t cleaning1 = make_cleaning(1, d);
  cleaning_t cleaning2 = make_cleaning(2, d);
  // Cleaning1 enters the dining hall and starts cleaning
  pthread_create(&cleaning1.thread, NULL, cleaning_enter, &cleaning1);

  msleep(100);
  // Student1 is waiting in line for cleaning1 to finish.
  pthread_create(&student1.thread, NULL, student_enter, &student1);

  msleep(100);
  // cleaning2 enters and waits in line, he should only clean after student1
  // finishes.
  pthread_create(&cleaning2.thread, NULL, cleaning_enter, &cleaning2);

  msleep(100);

  pthread_create(&student2.thread, NULL, student_enter, &student2);

  msleep(100);

  pthread_join(cleaning1.thread, NULL);
  cleaning_leave(&cleaning1);

  msleep(100);

  pthread_join(cleaning2.thread, NULL);
  cleaning_leave(&cleaning2);

  
  msleep(100);

  pthread_join(student1.thread, NULL);
  student_leave(&student1);

  msleep(100);


  pthread_join(student2.thread, NULL);
  student_leave(&student2);

  dining_destroy(&d);
}
