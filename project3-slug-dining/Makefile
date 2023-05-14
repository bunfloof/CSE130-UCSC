# (This is not the original Makefile file; please revert back on final submission.) \
Makes all files starting with example*.c

CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -Wno-unused-parameter
PROGS := $(patsubst %.c,%,$(wildcard example*.c))
OBJS := dining.o utils.o

all: $(PROGS)

$(PROGS): % : %.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	- rm -f *.o $(PROGS)

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: check-format
check-format:
	clang-format --dry-run --Werror *.c *.h
