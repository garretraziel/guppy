LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -lm -O3 -march=native
DFLAGS=-g -DDEBUG

.PHONY: rebuild build dep clean zip debug


build: dep $(PROJECT)

rebuild: clean build

debug: CFLAGS += $(DFLAGS)
debug: rebuild
jendebug: CFLAGS += -g
jendebug: rebuild

$(PROJECT): guppy.o string.o lexical.o parser.o ial.o errors.o expr.o exec.o
	$(CC) $(CFLAGS) $^ -o $@


# automaticke generovani zavislosti
dep: 
	$(CC) -MM *.c >dep.list

-include dep.list

clean:
	rm -f *.o $(PROJECT) dep.list

zip:
	cp ../doc/dokumentace.pdf ./
	tar czf $(LOGIN).tgz *.c *.h rozdeleni rozsireni dokumentace.pdf Makefile
