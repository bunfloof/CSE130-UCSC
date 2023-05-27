#!/usr/bin/env python3


from collections import defaultdict
from dataclasses import dataclass
import re
import sys


student_enter_regex = r"(Student \d+) entered"
student_leave_regex = r"(Student \d+) leaves"
cleaning_enter_regex = r"(Cleaning \d+) entered"
cleaning_leave_regex = r"(Cleaning \d+) leaves"


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


@dataclass
class Log:
    entered: int
    left: int


class DiningObserver:
    def __init__(self, capacity: int) -> None:
        self.capacity = capacity  # capacity of the DH
        self.num_students = 0  # number of students in the DH
        self.max_students = 0  # maximum number of students in the DH at a time
        self.num_cleaning = 0  # number of cleaning
        self.max_cleaning = 0  # maximum number of cleaners
        # set to True if both student and cleaner is in DH
        self.student_and_cleaning = False
        # timestamp
        self.t = 0
        self.logs = defaultdict(lambda: Log(0, 0))

    def record(self, log: str) -> None:
        lines = log.split("\n")
        for line in lines:
            self.t += 1
            m = re.match(student_enter_regex, line)
            if m:
                sid = m.group(1)
                self.logs[sid].entered = self.t
                self.num_students += 1
            m = re.match(student_leave_regex, line)
            if m:
                sid = m.group(1)
                self.logs[sid].left = self.t
                self.num_students -= 1
            m = re.match(cleaning_enter_regex, line)
            if m:
                cid = m.group(1)
                self.logs[cid].entered = self.t
                self.num_cleaning += 1
            m = re.match(cleaning_leave_regex, line)
            if m:
                cid = m.group(1)
                self.logs[cid].left = self.t
                self.num_cleaning -= 1

            self.max_students = max(self.max_students, self.num_students)
            self.max_cleaning = max(self.max_cleaning, self.num_cleaning)
            self.student_and_cleaning = self.student_and_cleaning or (
                self.num_students > 0 and self.num_cleaning > 0
            )

    def check(self) -> bool:
        # Rule 1: capacity
        if self.max_students > self.capacity:
            eprint(
                f"max student count {self.max_students} is larger than the capacity {self.capacity}"
            )
            return False
        # Rule 2: one cleaner at a time
        if self.max_cleaning > 1:
            eprint(f"max cleaning count {self.max_cleaning} is larger than 1")
            return False
        # Rule 3: no student while cleaning
        if self.student_and_cleaning:
            eprint(f"cleaner and student were in the dining hall at the same time")
            return False
        print("OK")
        return True


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} [CAPACITY]")
        exit(1)
    do = DiningObserver(int(sys.argv[1]))
    lines = ""
    for line in sys.stdin:
        lines += line
    if len(lines) <= 0:
        eprint("log not found")
        exit(1)
    do.record(lines)
    exit(0 if do.check() else 1)
