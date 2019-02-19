#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = client client_htonl client_udp mcast_client mcast_listener \
	mcast_sender mcast_server server server_htonl server_select \
	server_udp server_unix client_unix showip showname server_epoll \
	server_chat socket_pair server_udp_fortune server_unix_fd client_unix_fd \
	server_select_file server_epoll_fifo


all: $(PROGS)


client: client.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

client.o: client.c
	$(CC) $(CFLAGS) -c $<

client_htonl: client_htonl.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

client_htonl.o: client_htonl.c socket_htonl.h
	$(CC) $(CFLAGS) -c $<

client_udp: client_udp.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

client_udp.o: client_udp.c
	$(CC) $(CFLAGS) -c $<

mcast_client: mcast_client.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

mcast_client.o: mcast_client.c
	$(CC) $(CFLAGS) -c $<

mcast_listener: mcast_listener.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

mcast_listener.o: mcast_listener.c
	$(CC) $(CFLAGS) -c $<

mcast_sender: mcast_sender.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

mcast_sender.o: mcast_sender.c
	$(CC) $(CFLAGS) -c $<


server: server.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server.o: server.c
	$(CC) $(CFLAGS) -c $<

server_chat: server_chat.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_chat.o: server_chat.c
	$(CC) $(CFLAGS) -c $<

server_htonl: server_htonl.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_htonl.o: server_htonl.c
	$(CC) $(CFLAGS) -c $<

server_select: server_select.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_select.o: server_select.c
	$(CC) $(CFLAGS) -c $<

server_udp: server_udp.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_udp.o: server_udp.c
	$(CC) $(CFLAGS) -c $<



server_unix: server_unix.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_unix.o: server_unix.c
	$(CC) $(CFLAGS) -c $<

client_unix: client_unix.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

client_unix.o: client_unix.c
	$(CC) $(CFLAGS) -c $<


server_unix_fd: server_unix_fd.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_unix_fd.o: server_unix_fd.c
	$(CC) $(CFLAGS) -c $<

client_unix_fd: client_unix_fd.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

client_unix_fd.o: client_unix_fd.c
	$(CC) $(CFLAGS) -c $<


showip: showip.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

showip.o: showip.c
	$(CC) $(CFLAGS) -c $<

showname: showname.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

showname.o: showname.c
	$(CC) $(CFLAGS) -c $<

server_epoll: server_epoll.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_epoll.o: server_epoll.c
	$(CC) $(CFLAGS) -c $<

socket_pair: socket_pair.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

socket_pair.o: socket_pair.c
	$(CC) $(CFLAGS) -c $<

server_udp_fortune: server_udp_fortune.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_udp_fortune.o: server_udp_fortune.c
	$(CC) $(CFLAGS) -c $<


server_select_file: server_select_file.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_select_file.o: server_select_file.c
	$(CC) $(CFLAGS) -c $<


server_epoll_fifo: server_epoll_fifo.o
	$(CC) $(CFLAGS) -o $@ $^
	chmod a+rx,g-w $@

server_epoll_fifo.o: server_epoll_fifo.c
	$(CC) $(CFLAGS) -c $<


opt: clean
	$(MAKE) DEBUG=-O3
	strip $(PROGS)

# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#*

ci:
	git add *.c *.h [mM]akefile 
	git commit -m "lazy commit"

.PHONY: ci
.PHONY: clean
