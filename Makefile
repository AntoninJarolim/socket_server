CC=gcc
CFLAGS=-Wall -g

all: main.c
	$(CC) $(CFLAGS) -o hinfosvc main.c

.PHONY: clean
clean:
	rm -f *.o