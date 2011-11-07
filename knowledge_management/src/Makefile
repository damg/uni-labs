TARGET=kdm
OBJECTS=\
	main.o


all: compile

compile: $(TARGET)

clean:
	$(RM) -fv $(TARGET) $(OBJECTS) *~

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS)-o $@ $<
