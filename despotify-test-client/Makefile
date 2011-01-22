CC=gcc
DESPOTIFY ?= /home/ska/local

all: despotify-test

main.o: main.c
	$(CC) -std=c99 -I$(DESPOTIFY)/include -c -o $@ $<

despotify-test: main.o
	$(CC) -o $@ -L$(DESPOTIFY)/lib -ldespotify $<

clean:
	rm -f *.o *~ despotify-test
