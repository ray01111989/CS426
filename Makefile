CC=gcc
CFLAGS=-Wall

all: scopy
scopy: scopy.o
scopy.o: scopy.c functions.h

clean:
	rm -f scopy *.o

.PHONY: all clean
