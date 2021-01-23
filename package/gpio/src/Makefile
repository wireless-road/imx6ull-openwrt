all: gpio

CC = gcc
CFLAGS = -Wall
LDFLAGS = 

DEPS = $(wildcard *.h)

SRC = $(wildcard *.c)

OBJ = $(patsubst %.c, %.o, $(SRC))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gpio: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f gpio *.o

