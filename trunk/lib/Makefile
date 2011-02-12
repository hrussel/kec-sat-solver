CC=gcc

all: list.o sat.o sat

list.o: list.c list.h
	$(CC) -c list.c -g

sat: sat.o list.o
	$(CC) sat.o list.o -o sat -g
sat.o: sat.c sat.h list.h
	$(CC) -Wall -O3 -c sat.h sat.c -g

clean:
	rm -rf *.o sat
