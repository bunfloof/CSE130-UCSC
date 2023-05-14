CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -Wno-unused-parameter

all: example1 example2 example3 exampletest examplediscord1 examplediscord2 examplecodex1 examplecodex2

example1: example1.o dining.o utils.o
example2: example2.o dining.o utils.o
example3: example3.o dining.o utils.o
exampletest: exampletest.o dining.o utils.o
examplediscord1: examplediscord1.o dining.o utils.o
examplediscord2: examplediscord2.o dining.o utils.o
examplecodex1: examplecodex1.o dining.o utils.o
examplecodex2: examplecodex2.o dining.o utils.o

%.o : %.c
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	- rm -f *.o example1 example2 example3 exampletest examplediscord1 examplediscord2 examplecodex1 examplecodex2

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: check-format
check-format:
	clang-format --dry-run --Werror *.c *.h
