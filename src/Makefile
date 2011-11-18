LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

.PHONY: rebuild build dep clean zip

SOURCE_FILES=guppy.c string.c lexical.c parser.c ial.c

build: dep $(PROJECT) knuth-test parser-test

rebuild: clean build

$(PROJECT): guppy.o string.o lexical.o parser.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@


knuth-test: ial.o string.o knuth-test.o
	$(CC) $(CFLAGS) $^ -o $@

parser-test: ial.o string.o lexical.o parser.o parser-test.o
	$(CC) $(CFLAGS) $^ -o $@

# automaticke generovani zavislosti
dep: 
	$(CC) -MM *.c >dep.list

-include dep.list

clean:
	rm -f *.o $(PROJECT) lexical-test quicksort-test knuth-test parser-test substr-test dep.list

zip:
	tar czf $(LOGIN).tgz *.c *.h rozdeleni Makefile
