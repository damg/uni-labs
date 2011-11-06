SUBDIRS=lookahead yyless

all:
	set -e; for e in $(SUBDIRS); do $(MAKE) -C $$e; done

clean:
	set -e; for e in $(SUBDIRS); do $(MAKE) -C $$e clean; done

.PHONY: all clean .do_rule
