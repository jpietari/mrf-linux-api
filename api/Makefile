
CC=gcc

TARGETS := mmap_test simple evrsetup evan_monitor evr_fifo_monitor

APIOBJECTS := egapi.o erapi.o fctapi.o fracdiv.o sfpdiag.o

all: $(TARGETS) $(APIOBJECTS)

% : %.c

% : %.o $(APIOBJECTS)
	$(CC) $(LDFLAGS) -o $@ $< $(APIOBJECTS)

%.o : %.c $(APIDIR)/egapi.h $(APIDIR)/erapi.h $(APIDIR)/fctapi.h $(APIDIR)/fracdiv.h $(APIDIR)/sfpdiag.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~ $(TARGETS) html latex

doc:
	doxygen
	$(MAKE) -C latex -f Makefile
