LOGIN=xmokra00
PROJECT=guppy

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra


SOURCE_FILES=guppy.c


$(PROJECT): guppy.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@


clean:
	rm *.o $(PROJECT)

zip:
	tar cvzf $(LOGIN).tgz *.c *.h rozdeleni Makefile
