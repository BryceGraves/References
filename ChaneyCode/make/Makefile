CC = gcc
CFLAGS = -g -Wall
PROG = hellomake
TAR_FILE = goop.tar.gz
COMMENT = "This is a comment"

all: $(PROG)

hellomake: hellomake.o hellofunc.o
	$(CC) $(CFLAGS) -o hellomake hellomake.o hellofunc.o
	chmod og-rxw $(PROG)

hellomake.o: hellomake.c hellomake.h
	$(CC) $(CFLAGS) -c hellomake.c

hellofunc.o: hellofunc.c hellomake.h
	$(CC) $(CFLAGS) -c hellofunc.c

clean:
	rm -f $(PROG) *.o *~ \#*

tar:
	rm -f $(TAR_FILE)
	tar cvfa $(TAR_FILE) *.[ch] [Mm]akefile

comment:
	echo $(COMMENT)

opt:
	make CFLAGS=-O
