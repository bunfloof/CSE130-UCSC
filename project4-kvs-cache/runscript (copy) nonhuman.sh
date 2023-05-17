#!/bin/bash

client_commands=(
  "GET file1.txt"
  "GET file2.txt"
  "GET file3.txt"
  "GET file3.txt"
  "GET file1.txt"
)

commands=(
  "make clean"
  "make"
)

runCommand() {
  echo "💦 runscript.sh: Running $1"
  bash -c "$1"
  echo ""
}

for cmd in "${commands[@]}"; do
  runCommand "$cmd"
done

echo "💦 runscript.sh: Running ./client data FIFO 2"
{
  for client_cmd in "${client_commands[@]}"; do
    echo "💦 runscript.sh: Inputting: $client_cmd" >&2 # redirect to stderr (>&2) instead of stdout
    echo "$client_cmd"
  done
} | ./client data FIFO 2
echo ""
