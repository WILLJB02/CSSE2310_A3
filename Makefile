CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g
.PHONY: all clean
.DEFAULT_GOAL := all

all: client clientbot server




client:  client.o shared.o sharedClientFunctions.o
	$(CC) $(CFLAGS) $^ -o $@

client.o: client.c shared.h sharedClientFunctions.h



clientbot:  clientbot.o shared.o sharedClientFunctions.o

	$(CC) $(CFLAGS) $^ -o $@

clientbot.o: clientbot.c shared.h sharedClientFunctions.h

shared.o: shared.c shared.h

sharedClientFunctions.o: sharedClientFunctions.c sharedClientFunctions.h

server: server.o shared.o
	$(CC) $(CFLAGS) $^ -o $@



server.o: server.c shared.h


clean:
	rm server client clientbot
	rm *.o

