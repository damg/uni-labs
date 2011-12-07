CC=gcc
CFLAGS=-Wall
LDFLAGS=-pthread

TARGETS=ppr_mq ppr_pipe ppr_shmem

all: compile

compile: $(TARGETS)

ppr_pipe: ppr_pipe.o
	$(CC) $(LDFLAGS) -o $@ $<

ppr_mq: ppr_mq.o
	$(CC) $(LDFLAGS) -o $@ $<

ppr_shmem: ppr_shmem.o
	$(CC) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -fv $(OBJECTS) $(TARGETS)
