CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g -I../src/ -L../src

.PHONY: build rebuild clean

build: lexical-test quicksort-test substr-test knuth-test parser-test expr-test

rebuild: clean build

clean:
	rm -f *.o lexical-test quicksort-test knuth-test parser-test substr-test expr-test

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

expr-test: ../src/errors.o ../src/lexical.o ../src/string.o ../src/expr.o expr-test.o
	$(CC) $(CFLAGS) $^ -o $@

