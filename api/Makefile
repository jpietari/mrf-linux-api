
CC=gcc

all: mmap_test simple evrsetup

egapi.o: egapi.c egapi.h
	$(CC) $(CFLAGS) -c egapi.c

erapi.o: erapi.c erapi.h
	$(CC) $(CFLAGS) -c erapi.c

fracdiv.o: fracdiv.c fracdiv.h
	$(CC) $(CFLAGS) -c fracdiv.c

mmap_test.o: mmap_test.c egapi.h erapi.h egcpci.h
	$(CC) $(CFLAGS) -c mmap_test.c

mmap_test: mmap_test.o egapi.o erapi.o fracdiv.o
	$(CC) $(LDFLAGS) -o $@ mmap_test.o egapi.o erapi.o fracdiv.o

simple.o: simple.c egapi.h erapi.h egcpci.h
	$(CC) $(CFLAGS) -c simple.c

simple: simple.o egapi.o erapi.o fracdiv.o
	$(CC) $(LDFLAGS) -o $@ simple.o egapi.o erapi.o fracdiv.o

evrsetup.o: evrsetup.c egapi.h erapi.h egcpci.h
	$(CC) $(CFLAGS) -c evrsetup.c

evrsetup: evrsetup.o egapi.o erapi.o fracdiv.o
	$(CC) $(LDFLAGS) -o $@ evrsetup.o egapi.o erapi.o fracdiv.o

clean:
	rm -rf *.o *~ mmap_test simple evrsetup
