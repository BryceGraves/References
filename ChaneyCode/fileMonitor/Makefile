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
PROGS = dir_watcher nftw


all: $(PROGS)


dir_watcher: dir_watcher.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

dir_watcher.o: dir_watcher.c
	$(CC) $(CFLAGS) -c $<


nftw: nftw.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

nftw.o: nftw.c
	$(CC) $(CFLAGS) -c $<


opt: 
	$(MAKE) clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci:
	git add *.c [mM]akefile 
	git commit -m "lazy commit"

.PHONY: ci
.PHONY: clean

