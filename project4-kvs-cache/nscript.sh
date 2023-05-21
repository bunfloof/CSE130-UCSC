#!/bin/bash

# Define the directory, policy, and capacity
DIRECTORY="data"
POLICY="FIFO"
CAPACITY="2"

# Define the commands to send to your program
commands="
GET file1.txt
GET file2.txt
GET file3.txt
GET file3.txt
GET file1.txt
"

# Print the command being run
echo "./client $DIRECTORY $POLICY $CAPACITY"

# Print and run the program with the commands
printf "$commands" | tee /dev/fd/2 | ./client $DIRECTORY $POLICY $CAPACITY
