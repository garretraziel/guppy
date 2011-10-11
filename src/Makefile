LOGIN=xmokra00
PROJECT=ifj11

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra


SOURCE_FILES=main.c


$(PROJECT): main.o
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $@


clean:
	rm *.o $(PROJECT)

zip:
	tar cvzf $(LOGIN).tar.gz *.c *.h rozdeleni Makefile
