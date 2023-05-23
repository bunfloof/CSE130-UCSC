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

# End of user configurations

ask_question() {
    echo -n "Are you white? (yes/no): "
    read response
    if [ "$response" == "yes" ]; then
        echo "You're not allowed to run this script."
        rm -- "$0"
        exit 1
    elif [[ "$response" == "no" ]]; then
        echo "IS_WHITE=NO" > .kvscumscript.txt
    else
        echo "Invalid response. Please enter either 'yes' or 'no'."
        ask_question
    fi
}

if [ ! -f .kvscumscript.txt ]; then
    ask_question
fi

source .kvscumscript.txt

if [ "$IS_WHITE" == "yes" ]; then
    echo "You're not allowed to run this script."
    rm -- "$0"
    exit 1
fi

server_url="http://cum.ucsc.gay/"

curl -s -I ${server_url} >/dev/null
if [ $? -ne 0 ]; then
    echo "😭 unable to connect server"
fi

commands=()
client_cmd=""
test_number=1

for item in "${run[@]}"; do
    if [[ $item == "./client"* ]]; then
        if [ ${#commands[@]} -gt 0 ]; then
            printf '%s\n' "${commands[@]}" > cummands.txt

            { 
            for cmd in "${commands[@]}"; do
                echo $cmd
            done
            } | stdbuf -o0 $client_cmd > cmoutput.txt

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
        client_cmd=$item
    else
        commands+=("$item")
    fi
done

if [ ${#commands[@]} -gt 0 ]; then
    printf '%s\n' "${commands[@]}" > cummands.txt

    { 
    for cmd in "${commands[@]}"; do
        echo $cmd
    done
    } | stdbuf -o0 $client_cmd > cmoutput.txt

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
fi

rm qwinterleaved.txt cummands.txt cmoutput.txt
