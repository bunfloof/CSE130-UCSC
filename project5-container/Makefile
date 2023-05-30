CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -Wno-unused-parameter -Wno-unused-variable

TARGET=container
OBJECTS=container.o change_root.o

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	- rm -f *.o container

.PHONY: format
format:
	clang-format -i *.c *.h

.PHONY: check-format
check-format:
	clang-format --dry-run --Werror *.c *.h
