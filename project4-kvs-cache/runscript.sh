#!/bin/bash

# Configurations
EXPERIMENTAL_INTERLEAVE_MODE=true

# Define run array with client configurations and commands
run=(
"valgrind --leak-check=full ./client data FIFO 2"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
"GET file3.txt"
"GET file1.txt"
"valgrind --leak-check=full ./client data CLOCK 2"
"SET file1.txt hey"
"SET file2.txt hello"
"SET file3.txt hi"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
"valgrind --leak-check=full ./client data LRU 2"
"SET file1.txt hey"
"SET file2.txt hello"
"SET file3.txt hi"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
)

# End of user configurations

make || { echo '😭 make command failed' ; exit 1; }

server_url="http://cum.ucsc.gay/"

curl -s -I ${server_url} >/dev/null
if [ $? -ne 0 ]; then
    echo "😭 unable to connect server"
fi

commands=()
client_cmd=""
test_number=1

function execute_cmd {
    if [ ${#commands[@]} -gt 0 ]; then
        printf '%s\n' "${commands[@]}" > cummands.txt

        if [[ $client_cmd == "valgrind"* ]]; then
            bash -c "$client_cmd" < <(printf '%s\n' "${commands[@]}") > cmoutput.txt
        else
            { 
            for cmd in "${commands[@]}"; do
                echo $cmd
            done
            } | bash -c "$client_cmd" > cmoutput.txt
        fi

        if [ "$EXPERIMENTAL_INTERLEAVE_MODE" = true ] ; then
            curl -s -F 'commands.txt=@./cummands.txt' -F 'output.txt=@./cmoutput.txt' ${server_url} > qwinterleaved.txt
        else
            curl -s -F 'commands.txt=@./cummands.txt' -F 'output.txt=@./cmoutput.txt' -F 'append_mode=true' ${server_url} > qwinterleaved.txt
        fi

        echo
        echo "💦 Test $test_number:"
        echo $client_cmd
        cat qwinterleaved.txt | tr -d '[]"' | sed 's/, /\n/g'
        echo

        ((test_number++))

        commands=()
    fi
}

for item in "${run[@]}"; do
    if [[ $item == "./client"* || $item == "valgrind"* ]]; then
        execute_cmd
        client_cmd=$item
    else
        commands+=("$item")
    fi
done

execute_cmd

rm qwinterleaved.txt cummands.txt cmoutput.txt
