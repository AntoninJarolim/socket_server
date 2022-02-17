CC=gcc
CFLAGS=-Wall -g

all: build

build: main.c
	$(CC) $(CFLAGS) -o hinfosvc main.c
.PHONY: clean
clean:
	rm -f *.o

memcheck: build
	valgrind ./a