import time
import random
import os

NUMBER_OF_TRIALS = 1        # default 100
LOOPS_PER_TRIAL = 25         # default 30
MAX_HALLS = 3                # default 5
MAX_HALL_CAPACITY = 4       # default 10

MAX_STUDENTS_PER_HALL = 8   # default 20, keep <= 1000 (for variable naming purposes)
MIN_STUDENTS_PER_HALL = 0

MAX_CLEANERS_PER_HALL = 8   # default 20, keep <= 1000 (for variable naming purposes)
MIN_CLEANERS_PER_HALL = 0

SLEEP_TIME = 100             # default 100, may need to go higher for more DH/studs

FLUSH_LOG_FILE = True       # default True

if FLUSH_LOG_FILE: os.system('echo start: > log_file')

class Hall:
    def __init__(self, number, capacity, num_studs, num_clean, ordering_init):
        self.number = number
        self.capacity = capacity
        self.num_studs = num_studs
        self.num_clean = num_clean
        self.ordering = ordering_init

        self.s_state = {1000*number + i:None for i in range(1, num_studs+1)}
        self.c_state = {1000*number + i:None for i in range(1, num_clean+1)}

        self.slept = False
        self.wait_time = 0

    def students_in(self, state):
        return [student for student in self.s_state if self.s_state[student] == state]
    def cleaners_in(self, state):
        return [cleaner for cleaner in self.c_state if self.c_state[cleaner] == state]

    def students_made(self):
        return len(self.students_in('thread enter')+self.students_in('join enter')+self.students_in('enter sleep')+self.students_in('thread exit')+self.students_in('join exit'))
    def cleaners_made(self):
        return len(self.cleaners_in('thread enter')+self.cleaners_in('join enter')+self.cleaners_in('enter sleep')+self.cleaners_in('thread exit')+self.cleaners_in('join exit'))

    def add_action(self):
        choices = []

        if self.students_made() < self.num_studs:
            if len(self.students_in('thread enter')) < self.capacity: # queue < capacity
                if len(self.cleaners_in('thread enter')) == 0 or len(self.cleaners_in("enter sleep")) == 1: # no unslept cleaners OR 1 slept cleaner
                    choices.append("student thread enter")

        if len(self.students_in('thread enter')) > 0:
            if len(self.cleaners_in('thread enter')+self.cleaners_in('enter sleep')+self.cleaners_in('join enter')+self.cleaners_in("thread exit")) == 0:
                if len(self.students_in('thread enter')) <= self.capacity - len(self.students_in('join enter')):
                    choices.append("student join enter")

        if len(self.students_in('join enter')) > 0: # cant do it if we are at capacity and there is an unslept cleaner, same for cleaners
            if not (len(self.students_in('join enter')) == self.capacity and len(self.cleaners_in('thread enter')) == 1):
                choices.append("student thread exit")

        if len(self.students_in('thread exit')) > 0:
            choices.append('student join exit')


        if self.cleaners_made() < self.num_clean:
            if len(self.cleaners_in('thread enter')+self.cleaners_in('enter sleep')) == 0: # no cleaners in queue
                if len(self.students_in('join enter')) == self.capacity: # DH at capacity
                    choices.append('cleaner thread enter')
                elif len(self.students_in('thread enter')+self.students_in('enter sleep')) == 0:         # no students in queue
                    choices.append('cleaner thread enter')

        if len(self.cleaners_in('thread enter')+self.cleaners_in('enter sleep')) == 1:  # cleaner in queue
            if len(self.students_in('thread exit')+self.students_in('join enter')) == 0: # no pos students in DH
                if len(self.cleaners_in('join enter')+self.cleaners_in('thread exit')) == 0: # no cleaners in DH
                    choices.append('cleaner join enter')

        if len(self.cleaners_in('join enter')) == 1:
            choices.append('cleaner thread exit')

        if len(self.cleaners_in('thread exit')) == 1:
            choices.append('cleaner join exit')

        if not self.slept or len(self.cleaners_in('thread enter')) == 1:
            choices.append('msleep')

        if len(choices) == 0 or (choices == ['msleep'] and self.slept and not len(self.cleaners_in('thread enter'))): return False
        action = random.choice(choices)


        if action == 'msleep':
            self.append(f"msleep({SLEEP_TIME});\n")
            self.slept = True
            self.wait_time += SLEEP_TIME
            # same checks as for choices
            for student in self.students_in('thread enter'):
                if len(self.students_in('join enter')) == self.capacity or len(self.cleaners_in('enter sleep')) == 1:
                    break
                if len(self.cleaners_in('thread enter')+self.cleaners_in('enter sleep')+self.cleaners_in('join enter')+self.cleaners_in("thread exit")) == 0:
                    if len(self.students_in('thread enter')) <= self.capacity - len(self.students_in('join enter')):
                        self.s_state[student] = 'join enter'
                        self.append(f'pthread_join(student{student}.thread, NULL);\n')
            for student in self.students_in('thread exit'):
                self.s_state[student] = 'join exit'
                self.append(f'pthread_join(student{student}.thread, NULL);\n')

            for cleaner in self.cleaners_in('thread enter'):
                self.c_state[cleaner] = 'enter sleep'
            for cleaner in self.cleaners_in('enter sleep'):
                if len(self.students_in('join enter')+self.students_in('thread exit')) == 0:
                    if len(self.cleaners_in('thread enter')+self.cleaners_in('enter sleep')) == 1:
                        if len(self.students_in('thread exit')+self.students_in('join enter')) == 0:
                            if len(self.cleaners_in('join enter')+self.cleaners_in('thread exit')) == 0:
                                self.c_state[cleaner] = 'join enter'
                                self.append(f'pthread_join(cleaning{cleaner}.thread, NULL);\n')
            for cleaner in self.cleaners_in('thread exit'):
                self.c_state[cleaner] = 'join exit'
                self.append(f'pthread_join(cleaning{cleaner}.thread, NULL);\n')
            return True
        else:
            self.slept = False

        type = "student" if "student" in action else "cleaning"
        function = "_enter" if "enter" in action else "_leave"
        function = type+function

        if action == "student thread enter":
            full_id = self.students_in(None)[0]
            self.s_state[full_id] = 'thread enter'
        elif action == "student join enter":
            full_id = random.choice(self.students_in('thread enter'))
            self.s_state[full_id] = 'join enter'
        elif action == "student thread exit":
            full_id = random.choice(self.students_in('join enter'))
            self.s_state[full_id] = 'thread exit'
        elif action == "student join exit":
            full_id = random.choice(self.students_in('thread exit'))
            self.s_state[full_id] = 'join exit'

        elif action == "cleaner thread enter":
            full_id = self.cleaners_in(None)[0]
            self.c_state[full_id] = 'thread enter'
        elif action == "cleaner join enter":
            full_id = random.choice(self.cleaners_in('enter sleep')+self.cleaners_in('thread enter'))
            self.c_state[full_id] = 'join enter'
        elif action == "cleaner thread exit":
            full_id = random.choice(self.cleaners_in('join enter'))
            self.c_state[full_id] = 'thread exit'
        elif action == "cleaner join exit":
            full_id = random.choice(self.cleaners_in('thread exit'))
            self.c_state[full_id] = 'join exit'

        if "thread" in action:
            self.append(f'pthread_create(&{type}{full_id}.thread, NULL, {function}, &{type}{full_id});\n')
        elif "join" in action:
            self.append(f'pthread_join({type}{full_id}.thread, NULL);\n')
        
        return True

    def append(self, s):
        self.ordering.append(s)
    
    def has_more(self):
        return [self] if self.ordering else []
    def pop(self):
        return self.ordering.pop(0)

    def __repr__(self):
        return f"""\
        hall:        {self.number}
        capacity:    {self.capacity}
        total stud:  {self.num_studs}
        total clean: {self.num_clean}
        ordering:    {self.ordering}"""

start_time = time.time()
for trial in range(1, NUMBER_OF_TRIALS+1):
    num_halls = random.randint(1, MAX_HALLS)
    halls = []
    total_sleep_time = 0
    for hall_num in range(1, num_halls+1):
        number = hall_num
        max_cap = random.randint(1, MAX_HALL_CAPACITY)
        num_studs = random.randint(MIN_STUDENTS_PER_HALL, MAX_STUDENTS_PER_HALL)
        num_clean = random.randint(MIN_CLEANERS_PER_HALL, MAX_CLEANERS_PER_HALL)
        h = Hall(number, max_cap, num_studs, num_clean, [f"dining_t* d{number} = dining_init({max_cap});\n"])
        for student in range(1, num_studs+1):
            h.append(f"student_t student{1000*number + student} = make_student({1000*number + student}, d{number});\n")
        for cleaner in range(1, num_clean+1):
            h.append(f"cleaning_t cleaning{1000*number + cleaner} = make_cleaning({1000*number + cleaner}, d{number});\n")
        while True:
            if not h.add_action(): break
        h.append(f"dining_destroy(&d{hall_num});\n")
        halls.append(h)
        total_sleep_time += h.wait_time
    
    with open("example3.c", "w") as C:
        C.write('#include <pthread.h>\n#include <stdio.h>\n#include <unistd.h>\n#include "dining.h"\n#include "utils.h"\nint main(void) {\n')
        while True:
            options = []
            for h in halls:
                options += h.has_more()
            if not options: break
            hall = random.choice(options)
            statement = hall.pop()
            C.write(statement)
        C.write("}\n")
    
    os.system('cat example3.c >> log_file')
    os.system('make >> log_file') 

    print(f"This trial has {total_sleep_time/1000} seconds of sleep time. A mimir ðŸ˜´")
    for loop in range(1, LOOPS_PER_TRIAL+1):
        print(f"trial {trial}, loop {loop}")
        os.system('valgrind --leak-check=full --log-file=valgrind_out ./example3 > output_file')
        os.system(f'echo Trial {trial}, loop {loop} >> log_file')
        os.system('cat output_file >> log_file')
        with open("output_file", "r") as f:
            students_come = {i:0 for i in range(1, num_halls+1)}
            students_enter = {i:0 for i in range(1, num_halls+1)}
            students_leave = {i:0 for i in range(1, num_halls+1)}
            stud_statuses = {}

            cleaners_come = {i:0 for i in range(1, num_halls+1)}
            cleaners_enter = {i:0 for i in range(1, num_halls+1)}
            cleaners_leave = {i:0 for i in range(1, num_halls+1)}
            cleaner_statuses = {}

            cleaning = [False]*num_halls

            for line in f:
                l = line.split()
                full_id = l[1]
                dh = int(full_id[:-3])
                if "Student" in line:
                    if "comes in" in line:
                        students_come[dh] += 1
                        assert full_id not in stud_statuses
                        stud_statuses[full_id] = "come in"
                    elif "entered" in line:
                        students_enter[dh] += 1
                        assert students_enter[dh] <= students_come[dh]
                        assert students_enter[dh] -  students_leave[dh] <= halls[dh - 1].capacity
                        assert cleaners_enter[dh] == cleaners_leave[dh]
                        assert cleaning[dh - 1] == False
                        assert stud_statuses[full_id] == "come in"
                        stud_statuses[full_id] = "entered"
                    elif "leaves" in line:
                        students_leave[dh] += 1
                        assert students_leave[dh] <= students_enter[dh]
                        assert stud_statuses[full_id] == "entered"
                        stud_statuses[full_id] = "leaves"
                else:
                    if "comes in" in line:
                        cleaners_come[dh] += 1
                        assert full_id not in cleaner_statuses
                        cleaner_statuses[full_id] = "come in"
                    elif "entered" in line:
                        cleaners_enter[dh] += 1
                        assert cleaners_enter[dh] <= cleaners_come[dh]
                        assert cleaners_enter[dh] -  cleaners_leave[dh] == 1
                        assert students_enter[dh] == students_leave[dh]
                        assert cleaning[dh - 1] == False
                        cleaning[dh - 1] = True
                        assert cleaner_statuses[full_id] == "come in"
                        cleaner_statuses[full_id] = "entered"
                    elif "leaves" in line:
                        cleaners_leave[dh] += 1
                        assert cleaners_leave[dh] <= cleaners_enter[dh]
                        assert cleaning[dh - 1] == True
                        cleaning[dh - 1] = False
                        assert cleaner_statuses[full_id] == "entered"
                        cleaner_statuses[full_id] = "leaves"
                        assert cleaners_enter[dh] -  cleaners_leave[dh] == 0
            
            for hall_num in range(1, num_halls+1):
                assert students_come[hall_num] == students_enter[hall_num] == students_leave[hall_num] == halls[hall_num-1].num_studs
                assert cleaners_come[hall_num] == cleaners_enter[hall_num] == cleaners_leave[hall_num] == halls[hall_num-1].num_clean
                for stud_id in range(1, halls[hall_num-1].num_studs +1):
                    full_id = str(1000*hall_num + stud_id)
                    assert stud_statuses[full_id] == "leaves"
                for clean_id in range(1, halls[hall_num-1].num_clean +1):
                    full_id = str(1000*hall_num + clean_id)
                    assert cleaner_statuses[full_id] == "leaves"
        
        with open("valgrind_out", "r") as f:
            text = ''.join(f.readlines())
            assert "in use at exit: 0 bytes in 0 blocks" in text
            assert "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" in text
            assert "All heap blocks were freed -- no leaks are possible" in text
        
    print(f"time elapsed: {(time.time()-start_time)/60} minutes")
