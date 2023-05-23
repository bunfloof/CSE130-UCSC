#!/bin/bash

echo "staa"

# Define an array of commands
commands=(
"GET file1.txt"
"SET file2.txt"
"SET file3.txt foo"
"GET file3.txt"
"GET file1.txt"
)

# Write these commands into a text file
printf '%s\n' "${commands[@]}" > commands.txt

# Send these commands to the client and capture its output in a text file
{
for cmd in "${commands[@]}"; do
    echo $cmd
done
} | stdbuf -o0 ./client data FIFO 2 > output.txt

cat output.txt

rustc parse.rs

echo "💦 running parse.rs"
./parse