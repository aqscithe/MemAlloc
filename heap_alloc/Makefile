
CFLAGS=-O0 -g -Wall -Iinclude
CPPFLAGS=-MMD

OBJS=src/vmem.o src/perf.o src/heap.o

ALL_OBJS=$(OBJS) src/test.o
ALL_DEPS=$(ALL_OBJS:.o=.d)

.PHONY: all clean

all: test

-include $(ALL_DEPS)

test: $(OBJS) src/test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -rf $(ALL_OBJS) $(ALL_DEPS) test
