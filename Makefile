SUBDIRS := api wrapper

all clean:
	for dir in $(SUBDIRS); do \
          $(MAKE) -C $$dir -f Makefile $@; \
        done

.PHONY: all clean
