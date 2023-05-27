#!/bin/bash

# Configurations
EXPERIMENTAL_INTERLEAVE_MODE=true

# Define run array with client configurations and commands
run=(
"valgrind --leak-check=full ./client data FIFO 5"
"SET 6 New 6"
"SET 1 New 1"
"GET 3"
"GET 5"
"GET 1"
"GET 4"
"GET 2"
"GET 7"
"GET 3"
"SET 6 New 6"
"valgrind --leak-check=full ./client data FIFO 4"
"SET 3 New 3"
"GET 6"
"SET 3 New 3"
"SET 9 New 9"
"SET 8 New 8"
"GET 9"
"GET 6"
"GET 6"
"valgrind --leak-check=full ./client data CLOCK 4"
"GET 7"
"GET 4"
"GET 6"
"GET 7"
"GET 8"
"GET 3"
"SET 1 New 1"
"GET 6"
"valgrind --leak-check=full ./client data CLOCK 5"
"GET file7.txt"
"GET file0.txt"
"SET file4.txt hi"
"SET file7.txt hello"
"GET file8.txt"
"GET file3.txt"
"SET file2.txt hey"
"GET file8.txt"
"GET file9.txt"
"SET file7.txt hello"
"valgrind --leak-check=full ./client data FIFO 3"
"SET file1.txt file1"
"SET file2.txt file2"
"SET file3.txt file3"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
"valgrind --leak-check=full ./client data FIFO 3"
"SET file1.txt file1"
"SET file2.txt file2"
"SET file3.txt file3"
"SET file4.txt file4"
"GET file1.txt"
"GET file2.txt"
"GET file4.txt"
"SET file4.txt HI"
"SET file4.txt HEY"
"GET file1.txt"
"valgrind --leak-check=full ./client data LRU 3"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
"GET file1.txt"
"GET file3.txt"
"SET file4.txt HI"
"GET file2.txt"
"SET file2.txt hello"
"GET file1.txt"
"valgrind --leak-check=full ./client data CLOCK 3"
"GET fil1.txt"
"GET fil2.txt"
"GET fil3.txt"
"SET fil1.txt hi"
"GET fil1.txt"
"GET fil2.txt hey"
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
