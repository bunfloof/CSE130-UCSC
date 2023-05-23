#!/bin/bash

# Configurations
EXPERIMENTAL_INTERLEAVE_MODE=true

# Define run array with client configurations and commands
run=(
"./client data FIFO 2"
"GET file1.txt"
"SET file2.txt foo2"
"SET file3.txt foo"
"GET file3.txt"
"GET file1.txt"
"./client data LRU 2"
"GET file1.txt"
"GET file2.txt"
"GET file3.txt"
"./client data CLOCK 3"
"GET file1.txt"
"GET file2.txt"
)

if [ ! -f .kvscumscript.txt ]; then
    echo -n "Are you white? (yes/no): "
    read response
    if [ "$response" == "yes" ]; then
        echo "Sorry, you're not allowed to use this script."
        rm -- "$0"
        exit 1
    elif [[ "$response" == "no" ]]; then
        echo "IS_WHITE=NO" > .kvscumscript.txt
    else
        echo "Invalid response."
    fi
fi

source .kvscumscript.txt

if [ "$IS_WHITE" == "yes" ]; then
    echo "Sorry, you're not allowed to use this script."
    rm -- "$0"
    exit 1
fi

commands=()
client_cmd=""
test_number=1

for item in "${run[@]}"; do
    if [[ $item == "./client"* ]]; then
        if [ ${#commands[@]} -gt 0 ]; then
            printf '%s\n' "${commands[@]}" > commands.txt

            { 
            for cmd in "${commands[@]}"; do
                echo $cmd
            done
            } | stdbuf -o0 $client_cmd > output.txt

            if [ "$EXPERIMENTAL_INTERLEAVE_MODE" = true ] ; then
                curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' http://localhost:3030 > interleaved.txt
            else
                curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' -F 'append_mode=true' http://localhost:3030 > interleaved.txt
            fi

            echo
            echo "💦 Test $test_number:"
            echo $client_cmd
            cat interleaved.txt | tr -d '[]"' | sed 's/, /\n/g'
            echo

            ((test_number++))

            commands=()
        fi
        client_cmd=$item
    else
        commands+=("$item")
    fi
done

# Process remaining commands
if [ ${#commands[@]} -gt 0 ]; then
    printf '%s\n' "${commands[@]}" > commands.txt

    { 
    for cmd in "${commands[@]}"; do
        echo $cmd
    done
    } | stdbuf -o0 $client_cmd > output.txt

    if [ "$EXPERIMENTAL_INTERLEAVE_MODE" = true ] ; then
        curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' http://localhost:3030 > interleaved.txt
    else
        curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' -F 'append_mode=true' http://localhost:3030 > interleaved.txt
    fi

    echo
    echo "💦 Test $test_number:"
    echo $client_cmd
    cat interleaved.txt | tr -d '[]"' | sed 's/, /\n/g'
    echo
fi
