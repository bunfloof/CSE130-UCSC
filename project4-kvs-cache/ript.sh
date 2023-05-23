#!/bin/bash

commands=(
"GET file1.txt"
"SET file2.txt"
"SET file3.txt foo"
"GET file3.txt"
"GET file1.txt"
)

printf '%s\n' "${commands[@]}" > commands.txt

{
for cmd in "${commands[@]}"; do
    echo $cmd
done
} | stdbuf -o0 ./client data FIFO 2 > output.txt

curl -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' http://localhost:3030 > interleaved.txt

cat interleaved.txt