# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
#DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement -Wno-return-local-addr -Wuninitialized $(DEFINES)
PROGS = forking_exec forking_handler forking_sig forking_wait \
	forking_zombie forking_execvp forking_simple forking_bomb


all: $(PROGS)


forking_exec: forking_exec.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_exec.o: forking_exec.c
	$(CC) $(CFLAGS) -c $<

forking_execvp: forking_execvp.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_execvp.o: forking_execvp.c
	$(CC) $(CFLAGS) -c $<

forking_handler: forking_handler.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_handler.o: forking_handler.c
	$(CC) $(CFLAGS) -c $<

forking_sig: forking_sig.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_sig.o: forking_sig.c
	$(CC) $(CFLAGS) -c $<

forking_wait: forking_wait.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_wait.o: forking_wait.c
	$(CC) $(CFLAGS) -c $<

forking_zombie: forking_zombie.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_zombie.o: forking_zombie.c
	$(CC) $(CFLAGS) -c $<

forking_simple: forking_simple.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_simple.o: forking_simple.c
	$(CC) $(CFLAGS) -c $<

forking_bomb: forking_bomb.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

forking_bomb.o: forking_bomb.c
	$(CC) $(CFLAGS) -c $<


opt: 
	$(MAKE) clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

#ci:
#	git add *.c [mM]akefile
#	git commit -m "lazy commit"

.PHONY: ci
.PHONY: clean

ci:
	ci -m"auto-checkin" -l *.[ch] ?akefile

