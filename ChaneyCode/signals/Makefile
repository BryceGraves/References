#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = signals signal_catcher signal_sender signal_senderRT signal_catcherRT \
	signal_simple alarm stayin_alive


all: $(PROGS)


signals: signals.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signals.o: signals.c
	$(CC) $(CFLAGS) -c $<

signal_simple: signal_simple.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signal_simple.o: signal_simple.c
	$(CC) $(CFLAGS) -c $<


signal_catcher: signal_catcher.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signal_catcher.o: signal_catcher.c
	$(CC) $(CFLAGS) -c $<


signal_sender: signal_sender.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signal_sender.o: signal_sender.c
	$(CC) $(CFLAGS) -c $<


signal_catcherRT: signal_catcherRT.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signal_catcherRT.o: signal_catcherRT.c
	$(CC) $(CFLAGS) -c $<


signal_senderRT: signal_senderRT.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

signal_senderRT.o: signal_senderRT.c
	$(CC) $(CFLAGS) -c $<


alarm: alarm.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

alarm.o: alarm.c
	$(CC) $(CFLAGS) -c $<


stayin_alive: stayin_alive.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

stayin_alive.o: stayin_alive.c
	$(CC) $(CFLAGS) -c $<


opt: clean
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
