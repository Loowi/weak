CC=clang

CFLAGS=-g -pedantic -Wall -Werror -Wshadow -std=c99 -m64 -ltcmalloc -O3 -fomit-frame-pointer

all: weak

weak: $(wildcard *.c *.h)
	$(CC) $(CFLAGS) $(filter-out %.h, $^) -o $@

clean:
	rm -rf weak *.dSYM
