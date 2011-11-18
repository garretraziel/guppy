LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

.PHONY: rebuild build dep clean zip

SOURCE_FILES=guppy.c string.c lexical.c parser.c ial.c

build: dep $(PROJECT)

rebuild: clean build

$(PROJECT): guppy.o string.o lexical.o parser.o ial.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@


# automaticke generovani zavislosti
dep: 
	$(CC) -MM *.c >dep.list

-include dep.list

clean:
	rm -f *.o $(PROJECT) dep.list

zip:
	tar czf $(LOGIN).tgz *.c *.h rozdeleni Makefile
