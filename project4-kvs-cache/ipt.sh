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
