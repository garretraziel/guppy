LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g


SOURCE_FILES=guppy.c string.c lexical.c

build: $(PROJECT) lexical-test quicksort-test

$(PROJECT): guppy.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@

lexical-test: lexical.o lexical-test.o string.o
	$(CC) $(CFLAGS) $^ -o $@

quicksort-test: quicksort.o string.o quicksort-test.o
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm *.o $(PROJECT) lexical-test quicksort-test

zip:
	tar czf $(LOGIN).tgz *.c *.h rozdeleni Makefile
