LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g


SOURCE_FILES=guppy.c string.c lexical.c parser.c

build: dep $(PROJECT) lexical-test quicksort-test knuth-test

rebuild: clean build

$(PROJECT): guppy.o string.o lexical.o parser.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@

lexical-test: lexical.o lexical-test.o string.o
	$(CC) $(CFLAGS) $^ -o $@

quicksort-test: ial.o string.o quicksort-test.o
	$(CC) $(CFLAGS) $^ -o $@

knuth-test: ial.o string.o knuth-test.o
	$(CC) $(CFLAGS) $^ -o $@

# automaticke generovani zavislosti
dep: 
	$(CC) -MM *.c >dep.list

-include dep.list

clean:
	rm -f *.o $(PROJECT) lexical-test quicksort-test knuth-test dep.list

zip:
	tar czf $(LOGIN).tgz *.c *.h rozdeleni Makefile
