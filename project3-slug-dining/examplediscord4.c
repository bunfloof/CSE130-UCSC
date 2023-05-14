/* Logged from super */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "dining.h"
#include "utils.h"
// Tests for cleaners entering and not having to wait indefinitely if students arrive after (extra credit)
int main(void) {
    dining_t* d = dining_init(3);

    student_t student1 = make_student(1, d);
    student_t student2 = make_student(2, d);
    student_t student3 = make_student(3, d);
    student_t student4 = make_student(4, d);
    cleaning_t cleaning1 = make_cleaning(1, d);
    cleaning_t cleaning2 = make_cleaning(2, d);

    //Students 1,2,3 enter the dining hall. It is now full
    student_enter(&student1);
    student_enter(&student2);
    student_enter(&student3);

    msleep(100);
    //Student 4 attempts to eat, but cannot since it's full.
    pthread_create(&student4.thread, NULL, student_enter, &student4);

    msleep(100);
    //Now the cleaner comes in, he cannot since it's full.
    pthread_create(&cleaning1.thread, NULL, cleaning_enter, &cleaning1);

    msleep(100);
    //The dining hall empties out.
    student_leave(&student1);
    student_leave(&student2);
    student_leave(&student3);

    //Student 4 cannot enter the dining hall yet since the Cleaner was in the line. Cleaners have top priority.
    msleep(100);
    //The cleaner finishes cleaning. This means that Student 4 can now enter and start eating.
    pthread_join(cleaning1.thread, NULL);
    cleaning_leave(&cleaning1);

    msleep(100);
    
    //Now cleaning 2 comes in. Since this happens after an msleep, Student 4 should be able to enter first.
    pthread_create(&cleaning2.thread, NULL, cleaning_enter, &cleaning2);

    msleep(100);
    //Now Student 4 finishes eating and can leave the dining hall.
    pthread_join(student4.thread, NULL);
    student_leave(&student4);

    pthread_join(cleaning2.thread, NULL);
    cleaning_leave(&cleaning2);

    dining_destroy(&d);
}
