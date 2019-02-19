# R Jesse Chaney
# 

CC = gcc
DEBUG = -g
DEFINES = -DDEBUG

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement $(DEFINES)
PROGS = mystat read_file reverse rmchar sizeof write_file simple_seek read_simple checkTty statvfs nftw write_struct read_struct copy_file write_bytes


all: $(PROGS)

read_simple: read_simple.o
	$(CC) $(CFLAGS) -o $@ $^

read_simple.o: read_simple.c
	$(CC) $(CFLAGS) -c $<


simple_seek: simple_seek.o
	$(CC) $(CFLAGS) -o $@ $^

simple_seek.o: simple_seek.c
	$(CC) $(CFLAGS) -c $<


mystat: mystat.o
	$(CC) $(CFLAGS) -o $@ $^

mystat.o: mystat.c
	$(CC) $(CFLAGS) -c $<


read_file: read_file.o
	$(CC) $(CFLAGS) -o $@ $^

read_file.o: read_file.c
	$(CC) $(CFLAGS) -c $<


reverse: reverse.o
	$(CC) $(CFLAGS) -o $@ $^

reverse.o: reverse.c
	$(CC) $(CFLAGS) -c $<


rmchar: rmchar.o
	$(CC) $(CFLAGS) -o $@ $^

rmchar.o: rmchar.c rmchar.h
	$(CC) $(CFLAGS) -c $<


sizeof: sizeof.o
	$(CC) $(CFLAGS) -o $@ $^

sizeof.o: sizeof.c
	$(CC) $(CFLAGS) -c $<


write_file: write_file.o
	$(CC) $(CFLAGS) -o $@ $^

write_file.o: write_file.c
	$(CC) $(CFLAGS) -c $<


checkTty: checkTty.o
	$(CC) $(CFLAGS) -o $@ $^

checkTty.o: checkTty.c
	$(CC) $(CFLAGS) -c $<


statvfs: statvfs.o
	$(CC) $(CFLAGS) -o $@ $^

statvfs.o: statvfs.c
	$(CC) $(CFLAGS) -c $<

nftw: nftw.o
	$(CC) $(CFLAGS) -o $@ $^

nftw.o: nftw.c
	$(CC) $(CFLAGS) -c $<

write_struct: write_struct.o
	$(CC) $(CFLAGS) -o $@ $^

write_struct.o: write_struct.c rw_struct.h
	$(CC) $(CFLAGS) -c $<

read_struct: read_struct.o
	$(CC) $(CFLAGS) -o $@ $^

read_struct.o: read_struct.c rw_struct.h
	$(CC) $(CFLAGS) -c $<

copy_file: copy_file.o
	$(CC) $(CFLAGS) -o $@ $^

copy_file.o: copy_file.c
	$(CC) $(CFLAGS) -c $<

write_bytes: write_bytes.o
	$(CC) $(CFLAGS) -o $@ $^

write_bytes.o: write_bytes.c
	$(CC) $(CFLAGS) -c $<


# clean up the compiled files and editor chaff
clean:
	rm -f $(PROGS) *.o *~ \#* rev_numbers.txt
