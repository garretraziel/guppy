CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -I../src/ -L../src
DFLAGS=-g -DDEBUG

.PHONY: build rebuild clean guppy

build: guppy lexical-test quicksort-test substr-test knuth-test parser-test expr-test exec-test

rebuild: clean build

debug: CFLAGS += $(DFLAGS)
debug: DEBUG=debug
debug: rebuild

clean:
	rm -f *.o lexical-test quicksort-test knuth-test parser-test substr-test expr-test

guppy:
	make -C ../src/ -f ../src/Makefile $(DEBUG)

lexical-test: ../src/lexical.o ../src/string.o ../src/errors.o lexical-test.o
	$(CC) $(CFLAGS) $^ -o $@

quicksort-test: ../src/ial.o ../src/string.o ../src/errors.o quicksort-test.o
	$(CC) $(CFLAGS) $^ -o $@

substr-test: ../src/ial.o ../src/string.o ../src/errors.o substr-test.o
	$(CC) $(CFLAGS) $^ -o $@

knuth-test: ../src/ial.o ../src/string.o ../src/errors.o knuth-test.o
	$(CC) $(CFLAGS) $^ -o $@

parser-test: ../src/ial.o ../src/string.o ../src/lexical.o ../src/parser.o ../src/errors.o ../src/expr.o parser-test.o
	$(CC) $(CFLAGS) $^ -o $@

expr-test: ../src/errors.o ../src/lexical.o ../src/string.o ../src/expr.o ../src/parser.o ../src/ial.o expr-test.o 
	$(CC) $(CFLAGS) $^ -o $@

exec-test: ../src/exec.o ../src/string.o ../src/ial.o exec-test.o
	$(CC) $(CFLAGS) -lm $^ -o $@