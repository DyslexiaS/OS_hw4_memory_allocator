TARGETS = hw_malloc.o
CC ?= gcc
CFLAGS += -std=gnu99 -Wall
OBJS = $(TARGETS)

all: $(TARGETS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
astyle:
	 astyle --style=linux --indent=tab --max-code-length=80 --suffix=none lib/hw_malloc.h lib/hw_malloc.c hw4_mm_test.c hw4_mm_test.h
