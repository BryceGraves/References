# R Jesse Chaney
# Jesse@JesseChaney.net
# Copyright 2014 2015
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = user_procs


all: $(PROGS)


user_procs: user_procs.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

user_procs.o: user_procs.c
	$(CC) $(CFLAGS) -c $<

opt: 
	$(MAKE) clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile
