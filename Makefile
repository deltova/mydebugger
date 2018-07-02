CC=gcc
src = $(wildcard src/*.c)
obj = $(src:.c=.o)

CFLAGS = -std=c99 -pedantic -Wall -Wextra  -g3 -Isrc
LDFLAGS = -lbfd -lreadline

myprog: $(obj)
	$(CC) $(CFLAGS)  -o  $@ $^ $(LDFLAGS)
PHONY: clean
clean:
	rm -f $(obj) myprog
