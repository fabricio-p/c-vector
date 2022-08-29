CC ?= gcc
CFLAGS += -std=c99 -I . -I cake_libs/                                \
		  		-Wall -Wextra -Wformat=2 -Wshadow                          \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs  \
		  		-Wno-unused-parameter -Wno-unused-command-line-argument    \
		  		-Wno-missing-braces -Wno-unused-function -O0 -ggdb -lcunit

.PHONY: tests cvector_test cvector_pointermode_test clean

tests: cvector_test cvector_pointermode_test

cvector_test: test/cvector_test.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -lcunit -o bin/$@ $^

cvector_pointermode_test: test/cvector_pointermode_test.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -lcunit -o bin/$@ $^

clean:
	@if [ -d bin ]; then rm -rf bin; fi
