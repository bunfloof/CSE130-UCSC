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

coproc CLIENT_PROCESS { stdbuf -o0 ./client data FIFO 2; }

for client_cmd in "${client_commands[@]}"; do
  echo "$client_cmd"
  echo "$client_cmd" >&"${CLIENT_PROCESS[1]}"
  read -r output <&"${CLIENT_PROCESS[0]}"
  echo "$output"
done

exec {CLIENT_PROCESS[1]}>&-

while read -r output <&"${CLIENT_PROCESS[0]}"; do
  echo "$output"
done

wait $!
