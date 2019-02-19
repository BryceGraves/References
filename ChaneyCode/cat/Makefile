#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = my_cat1 my_cat2 my_cat3 my_cat4 my_tee my_seek


all: $(PROGS)


my_cat1: my_cat1.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_cat1.o: my_cat1.c
	$(CC) $(CFLAGS) -c $<

my_cat2: my_cat2.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_cat2.o: my_cat2.c
	$(CC) $(CFLAGS) -c $<

my_cat3: my_cat3.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_cat3.o: my_cat3.c
	$(CC) $(CFLAGS) -c $<

my_cat4: my_cat4.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_cat4.o: my_cat4.c
	$(CC) $(CFLAGS) -c $<

my_seek: my_seek.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_seek.o: my_seek.c
	$(CC) $(CFLAGS) -c $<

my_tee: my_tee.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

my_tee.o: my_tee.c
	$(CC) $(CFLAGS) -DMAX_TEE_FILES=10 -DMAX_BUFFER_LEN=1024 -c $<


opt: clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile
