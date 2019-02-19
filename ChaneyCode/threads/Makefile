#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = -DDEBUG -pthread

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = thread_mutex thread_barrier thread_cv1 thread_cv2 thread_simple thread_detach \
	thread_deadlock thread_mutex_timeout thread_trylock thread_rw \
	thread_mutex_first thread_safety thread_cvBucket


all: $(PROGS)


thread_simple: thread_simple.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_simple.o: thread_simple.c
	$(CC) $(CFLAGS) -c $<

thread_cv1: thread_cv1.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_cv1.o: thread_cv1.c
	$(CC) $(CFLAGS) -c $<

thread_cv2: thread_cv2.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_cv2.o: thread_cv2.c
	$(CC) $(CFLAGS) -c $<

thread_mutex: thread_mutex.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_mutex.o: thread_mutex.c
	$(CC) $(CFLAGS) -c $<

thread_barrier: thread_barrier.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_barrier.o: thread_barrier.c
	$(CC) $(CFLAGS) -c $<

thread_detach: thread_detach.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_detach.o: thread_detach.c
	$(CC) $(CFLAGS) -c $<

thread_deadlock: thread_deadlock.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_deadlock.o: thread_deadlock.c
	$(CC) $(CFLAGS) -c $<

# This needs an additional library to link.
thread_mutex_timeout: thread_mutex_timeout.o
	$(CC) $(CFLAGS) -o $@ $^ -lrt
	chmod a+rx,g-w $@

thread_mutex_timeout.o: thread_mutex_timeout.c
	$(CC) $(CFLAGS) -c $<

thread_trylock: thread_trylock.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_trylock.o: thread_trylock.c
	$(CC) $(CFLAGS) -c $<

thread_rw: thread_rw.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_rw.o: thread_rw.c
	$(CC) $(CFLAGS) -c $<

thread_mutex_first: thread_mutex_first.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_mutex_first.o: thread_mutex_first.c
	$(CC) $(CFLAGS) -c $<

thread_safety: thread_safety.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_safety.o: thread_safety.c
	$(CC) $(CFLAGS) -c $<

thread_cvBucket: thread_cvBucket.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

thread_cvBucket.o: thread_cvBucket.c
	$(CC) $(CFLAGS) -c $<


opt: clean
	$(MAKE) DEBUG=-O3 DEFINES=
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean cls:
	rm -f $(PROGS) *.o *~ \#*

ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile

#	git add *.[ch] [mM]akefile
#	git commit -m "lazy commit"

