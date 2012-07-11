CC=clang

COMMON_FLAGS=-g -pedantic -Wall -Werror -Wshadow -std=c99 -m64 -ltcmalloc
CFLAGS=$(COMMON_FLAGS) -O3 -fomit-frame-pointer
DEBUG_FLAGS=$(COMMON_FLAGS) -O0

CODE_FILES=$(wildcard *.c *.h)
TEST_FILES=$(CODE_FILES) $(wildcard tests/*.c tests/*.h)

all: weak

weak: $(CODE_FILES)
	$(CC) $(CFLAGS) $(filter-out %.h, $^) -o $@

clean:
	rm -rf weak tests/test *.dSYM tests/*.dSYM

debug: $(CODE_FILES)
	$(CC) $(DEBUG_FLAGS) $(filter-out %.h, $^) -o weak

# Typically, we don't want to run long-running tests. Default to QUICK_TEST.
test: $(TEST_FILES)
	$(CC) $(CFLAGS) -DQUICK_TEST $(filter-out main.c %.h, $^) -o tests/test
	./tests/test

# However, if pedantry is required, we have this :-)
testfull: $(TEST_FILES)
	$(CC) $(CFLAGS) $(filter-out main.c %.h, $^) -o tests/test
	./tests/test

.PHONY: all clean debug foo test testfull
