if [ $# -ne 1 ]
then
    echo "Call with directory"
    exit
fi

DIR=$1

echo $DIR

for item in $(ls $DIR)
do
    echo "$item: $(cat $DIR/$item)"
done