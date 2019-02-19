# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = pipes pipes_files rev_rev fifo_demo first_last pipe_capacity popen fifo_chat zhead


all: $(PROGS)


pipes: pipes.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

pipes.o: pipes.c
	$(CC) $(CFLAGS) -c $<

pipes_files: pipes_files.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

pipes_files.o: pipes_files.c
	$(CC) $(CFLAGS) -c $<

rev_rev: rev_rev.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

rev_rev.o: rev_rev.c
	$(CC) $(CFLAGS) -c $<

fifo_demo: fifo_demo.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

fifo_demo.o: fifo_demo.c
	$(CC) $(CFLAGS) -c $<

first_last: first_last.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

first_last.o: first_last.c
	$(CC) $(CFLAGS) -c $<

pipe_capacity: pipe_capacity.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

pipe_capacity.o: pipe_capacity.c
	$(CC) $(CFLAGS) -c $<

popen: popen.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

popen.o: popen.c
	$(CC) $(CFLAGS) -c $<

fifo_chat: fifo_chat.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

fifo_chat.o: fifo_chat.c
	$(CC) $(CFLAGS) -c $<

zhead: zhead.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@
	rm -f ztail
	ln zhead ztail

zhead.o: zhead.c
	$(CC) $(CFLAGS) -c $<


opt: 
	$(MAKE) clean
	$(MAKE) DEBUG=-O3 DEFINES=
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean cls:
	rm -f $(PROGS) ztail *.o *~ \#*

ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile
