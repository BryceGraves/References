#
# R Jesse Chaney
# Jesse@JesseChaney.net
# Copyright 2014 2015
#

CC = gcc
DEBUG = -g

# enable lots of warning messages.
# i happen to like bcpl style comments, so i cannot use -pedantic
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
	-Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
	-Wdeclaration-after-statement
PROGS= tennisMatch_SysV \
	tennisMatch_POSIX \
	tennisMatch_PThreads \
	tennisMatch_Pipes \
	tennisMatch_Signals

all: $(PROGS)


# this version of the application uses SysV message queues
tennisMatch_SysV: tennisMatch_SysV.o tennisMatch_Common.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

tennisMatch_SysV.o: tennisMatch_SysV.c tennisMatch_SysV.h tennisMatch_Common.h
	$(CC) $(CFLAGS) -c $<

# this version of the application uses POSIX message queues
tennisMatch_POSIX: tennisMatch_POSIX.o tennisMatch_Common.o
	$(CC) $(CFLAGS) -o $@ $^ -lrt
	chmod a+rx,g-w $@

tennisMatch_POSIX.o: tennisMatch_POSIX.c tennisMatch_POSIX.h tennisMatch_Common.h
	$(CC) $(CFLAGS) -c $<

# this version of the application uses POSIX Threads
tennisMatch_PThreads: tennisMatch_PThreads.o tennisMatch_Common.o
	$(CC) $(CFLAGS) -pthread -o $@ $^ -lrt
	chmod a+rx,g-w $@

tennisMatch_PThreads.o: tennisMatch_PThreads.c tennisMatch_PThreads.h tennisMatch_Common.h
	$(CC) $(CFLAGS) -pthread -c $<

# this version of the application uses unnamed Pipes
tennisMatch_Pipes: tennisMatch_Pipes.o tennisMatch_Common.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

tennisMatch_Pipes.o: tennisMatch_Pipes.c tennisMatch_Pipes.h tennisMatch_Common.h
	$(CC) $(CFLAGS) -c $<

# this version of the application uses Signals
tennisMatch_Signals: tennisMatch_Signals.o tennisMatch_Common.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

tennisMatch_Signals.o: tennisMatch_Signals.c tennisMatch_Signals.h tennisMatch_Common.h
	$(CC) $(CFLAGS) -c $<

# this module is common to all programs.
tennisMatch_Common.o: tennisMatch_Common.c tennisMatch_Common.h
	$(CC) $(CFLAGS) -c $<

# clean up the compiled files
clean:
	rm -f $(PROGS) *.o *~ \#*
