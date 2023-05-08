CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic

.PHONY: all
all: word-count mr.o

word-count: word-count.c kvlist.o mr.o hash.o

%.o : %.c %.h
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	-rm word-count *.o

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: check-format
check-format:
	clang-format --dry-run --Werror *.c *.h
