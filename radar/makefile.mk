# Target: Prerequisites
# [Tab] Command

a: main.o server.o
	gcc main.o server.o -o a

main.o: main.c server.h
	gcc -c main.c

server.o: server.c server.h
	gcc -c server.c