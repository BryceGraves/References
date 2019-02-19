#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = clientshm clientshm_unnamed clientshm_named \
	servershm servershm_unnamed servershm_named shm_array
LIBS = -lrt -pthread

all: $(PROGS)


clientshm: clientshm.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

clientshm.o: clientshm.c shm.h
	$(CC) $(CFLAGS) -c $<

clientshm_unnamed: clientshm_unnamed.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

clientshm_unnamed.o: clientshm_unnamed.c shm.h
	$(CC) $(CFLAGS) -c $<

clientshm_named: clientshm_named.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

clientshm_named.o: clientshm_named.c shm.h
	$(CC) $(CFLAGS) -c $<

servershm: servershm.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

servershm.o: servershm.c shm.h
	$(CC) $(CFLAGS) -c $<

servershm_unnamed: servershm_unnamed.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

servershm_unnamed.o: servershm_unnamed.c shm.h
	$(CC) $(CFLAGS) -c $<

servershm_named: servershm_named.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

servershm_named.o: servershm_named.c shm.h
	$(CC) $(CFLAGS) -c $<

shm_array: shm_array.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	chmod a+rx,g-w $@

shm_array.o: shm_array.c shm.h
	$(CC) $(CFLAGS) -c $<


opt: clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci: clean
	ci -m"auto-checkin" -l *.[ch] ?akefile
