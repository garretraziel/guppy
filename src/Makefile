LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -lm
DFLAGS=-g -DDEBUG

.PHONY: rebuild build dep clean zip debug


build: dep $(PROJECT)

rebuild: clean build

debug: CFLAGS += $(DFLAGS)
debug: rebuild

$(PROJECT): guppy.o string.o lexical.o parser.o ial.o errors.o expr.o exec.o
	$(CC) $(CFLAGS) $^ -o $@


# automaticke generovani zavislosti
dep: 
	$(CC) -MM *.c >dep.list

-include dep.list

clean:
	rm -f *.o $(PROJECT) dep.list

zip:
	tar czf $(LOGIN).tgz *.c *.h rozdeleni Makefile
