curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' http://localhost:3030 > interleaved.txt
cat interleaved.txt

curl -s -F 'commands.txt=@./commands.txt' -F 'output.txt=@./output.txt' -F 'append_mode=true' http://localhost:3030 > appended.txt
cat appended.txt