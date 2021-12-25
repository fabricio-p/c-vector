CFLAGS ?= -I ./ -std=c99 -DDEBUG -Wall -Wextra -Ofast -Wno-unused-function -Wno-unused-variable

tests: cvector_test cvector_pointermode_test

cvector_test: test/cvector_test.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -lcunit -o bin/$@ $^

cvector_pointermode_test: test/cvector_pointermode_test.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -lcunit -o bin/$@ $^

clean:
	@if [ -d bin ]; then rm -rf bin; fi
