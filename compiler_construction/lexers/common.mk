CC=gcc
CFLAGS=-Wall
LDFLAGS=
LEX=flex

all: compile

compile: $(TARGET)

$(TARGET): $(TARGET).yy.o
	$(CC) $(LDFLAGS) -o $@ $<

%.yy.o: %.yy.c
	$(CC) $(CFLAGS) -c $<

%.yy.c: %.l
	$(LEX) -o $@ $<

%.l: ../common.inc.c

clean:
	$(RM) -fv *.o *.yy.c *~ $(TARGET)

.PHONY: all compile clean
