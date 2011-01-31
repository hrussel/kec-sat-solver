CC=gcc

all: list.o sat.o sat

list.o: list.c list.h
	$(CC) -c list.c

sat: sat.o list.o
	$(CC) sat.o list.o -o sat
sat.o: sat.c sat.h list.h
	$(CC) -c sat.h sat.c

clean:
	rm -rf *.o sat
