#!/bin/bash
# ./penis.sh -f quotes.txt

#!/bin/bash

while getopts "f:" opt; do
  case ${opt} in
    f )
      file=$OPTARG
      ;;
    \? )
      echo "😭 invalid option: -$OPTARG" 1>&2
      exit 1
      ;;
  esac
done

if [ ! -f "$file" ]; then
  echo "😭 $file not found"
  exit 1
fi

[[ $(tail -c1 "$file" | wc -l) -eq 0 ]] && echo "" >> "$file"

sed 's/[[:space:]]*$//' $file | while IFS= read -r line; do
  echo "\"$line\""
done
