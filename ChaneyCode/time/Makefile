#
# R Jesse Chaney
# Jesse@JesseChaney.net
# Copyright 2014 2015
# 

CC = gcc
DEBUG = -g
#DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = mytime mygettimeofday show_time


all: $(PROGS)


mytime: mytime.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

mytime.o: mytime.c
	$(CC) $(CFLAGS) -c $<

mygettimeofday: mygettimeofday.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

mygettimeofday.o: mygettimeofday.c
	$(CC) $(CFLAGS) -c $<

show_time: show_time.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

show_time.o: show_time.c
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
