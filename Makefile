CC=gcc

all: list.o sat.o sat

list.o: list.c list.h
	$(CC) -c list.c -g

sat.o: sat.c sat.h list.h
	$(CC) -Wall -O3 -c sat.h sat.c -g

sat_io.o: sat_io.c sat.h list.h
	$(CC) -Wall -O3 -c sat_io.c sat.h

main.o: main.c sat.h list.h
	$(CC) -Wall -O3 -c main.c sat.h -g

sat: main.o sat.o sat_io.o list.o
	$(CC) main.o sat.o sat_io.o list.o -o sat -g

clean:
	rm -rf *.o sat
