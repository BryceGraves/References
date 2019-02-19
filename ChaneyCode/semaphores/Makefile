# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement -pthread $(DEFINES)
PROGS = simple_sem simple_sem_multi simple_sem_thread

all: $(PROGS)


simple_sem: simple_sem.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

simple_sem.o: simple_sem.c
	$(CC) $(CFLAGS) -c $<

simple_sem_multi: simple_sem_multi.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

simple_sem_multi.o: simple_sem_multi.c
	$(CC) $(CFLAGS) -c $<

simple_sem_thread: simple_sem_thread.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

simple_sem_thread.o: simple_sem_thread.c
	$(CC) $(CFLAGS) -c $<


# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) a.out *.o *~ \#*

ci:
	git add *.c [mM]akefile 
	git commit -m "lazy commit"

.PHONY: ci
.PHONY: clean

