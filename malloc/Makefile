
BIN=string_test
CFLAGS=-Wall -O0 -g
CPPFLAGS=-MMD

OBJS=src/main.o src/lstring.o src/sstring.o
DEPS=$(OBJS:.o=.d)

.PHONY: all clean
all: $(BIN)

-include $(DEPS)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(BIN)
