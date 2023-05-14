# Retrieved by NGX on May 13 2023; logged from super

if [ $# -ne 2 ]
then
    echo "Incorrect parameter usage."
    echo "Correct usage: sh repeat.sh [executable name] [number of trials]"
    exit
fi
name="$1"
NUM_TRIALS=$2

echo "Running Base Case first"

timeout 3 $name > base.txt

if [ $? -eq 124 ]
then
    echo "$name timed out. Likely due to Deadlock."
    exit
fi
echo "Contents of Base case are:"
echo "------------------------------"
cat base.txt
echo "------------------------------"


echo "Now repeat $NUM_TRIALS times"
for count in `seq 2 $NUM_TRIALS` 
do
    timeout 3 $name > loopout.txt
    if [ $? -eq 124 ]
    then
        echo "$name timed out on trial $count"
        exit
    fi
    diff loopout.txt base.txt > diff.txt
    if [ $? -eq 1 ]
    then
        echo "Your case was different, please check diff.txt."
        echo "There is a chance that your produced output is still correct due to processes being woken up in an uncontrollable order."
        exit
    fi
    echo "Trial $count passed"

done
