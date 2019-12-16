CC=gcc 
CFLAGS=-Wall

all: scopy
scopy: scopy.o
scopy.o: scopy.c functions.h

clean:
	rm scopy *.o

