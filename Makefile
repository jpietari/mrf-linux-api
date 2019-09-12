SUBDIRS := api wrapper

all clean doc:
	for dir in $(SUBDIRS); do \
          $(MAKE) -C $$dir -f Makefile $@; \
        done

.PHONY: all clean doc
