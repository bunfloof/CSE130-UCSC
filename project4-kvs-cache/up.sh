curl -X POST -d "$(cat commands.txt; echo -e '\n---\n'; cat output.txt)" http://localhost:8000/upload > interleaved.txt
