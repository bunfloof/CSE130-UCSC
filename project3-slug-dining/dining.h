#pragma once

struct dining;
typedef struct dining dining_t;

dining_t *dining_init(int capacity);
void dining_destroy(dining_t **dining);

void dining_student_enter(dining_t *dining);
void dining_student_leave(dining_t *dining);

void dining_cleaning_enter(dining_t *dining);
void dining_cleaning_leave(dining_t *dining);
