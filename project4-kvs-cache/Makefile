CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -Wno-unused-parameter

TARGET=client
OBJECTS=client.o kvs.o kvs_base.o kvs_clock.o kvs_fifo.o kvs_lru.o

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	- rm -f *.o client

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: check-format
check-format:
	clang-format --dry-run --Werror *.c *.h
