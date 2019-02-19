# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = argv envp environ getopt_short getopt_long loop

all: $(PROGS)


argv: argv.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

argv.o: argv.c
	$(CC) $(CFLAGS) -c $<


envp: envp.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

envp.o: envp.c
	$(CC) $(CFLAGS) -c $<


environ: environ.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

environ.o: environ.c
	$(CC) $(CFLAGS) -c $<


getopt_short: getopt_short.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

getopt_short.o: getopt_short.c
	$(CC) $(CFLAGS) -c $<


getopt_long: getopt_long.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

getopt_long.o: getopt_long.c
	$(CC) $(CFLAGS) -c $<


loop: loop.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

loop.o: loop.c
	$(CC) $(CFLAGS) -c $<

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci:
	ci -l *.[ch] ?akefile
