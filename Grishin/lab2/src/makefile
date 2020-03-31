CC = g++
CFLAGS = -g -std=c++17


all: main

main: main.o edge.o vertex.o
	$(CC) $(CFLAGS) main.o edge.o vertex.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

edge.o: edge.h edge.cpp
	$(CC) $(CFLAGS) -c edge.cpp

vertex.o: vertex.h vertex.cpp
	$(CC) $(CFLAGS) -c vertex.cpp

clean:
	rm -rf *.o main
