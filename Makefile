CFLAGS ?= -I ./ -std=c99 -DDEBUG -Wall -Wextra -Ofast -Wno-unused-function -Wno-unused-variable

tests: cvector_test cvector_pointermode_test

cvector_test: test/cvector_test.c
	$(CC) $(CFLAGS) -lcunit -o test/$@ $^

cvector_pointermode_test: test/cvector_pointermode_test.c
	$(CC) $(CFLAGS) -lcunit -o test/$@ $^

clean:
	@for f in $$(find test/ -name "*.o")\
		$$(find test/ -type f -executable) ; \
		do rm $$f ; \
	done
