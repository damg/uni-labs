SUBDIRS=lookahead yyless reject

all:
	set -e; for e in $(SUBDIRS); do $(MAKE) -C $$e; done

clean:
	$(RM) *~
	set -e; for e in $(SUBDIRS); do $(MAKE) -C $$e clean; done

test: all
	./test.sh

.PHONY: all clean test
