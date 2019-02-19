# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = getpwent getgrent getusers

all: $(PROGS)

getpwent: getpwent.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

getpwent.o: getpwent.c
	$(CC) $(CFLAGS) -c $<

getgrent: getgrent.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

getgrent.o: getgrent.c
	$(CC) $(CFLAGS) -c $<

getusers: getusers.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

getusers.o: getusers.c
	$(CC) $(CFLAGS) -c $<


# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#* 

ci:
	ci -l -m"Makefile checkin" *.[ch] ?akefile
