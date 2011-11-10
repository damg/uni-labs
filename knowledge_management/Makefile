CC=gcc
CFLAGS=-Wall -g -ggdb
LDFLAGS=

SUBDIRS=src

all:
	for e in $(SUBDIRS); do $(MAKE) -C $$e; done

clean:
	for e in $(SUBDIRS); do $(MAKE) -C $$e clean; done
