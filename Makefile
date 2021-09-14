CFLAGS ?= -I ./ -std=c99 -DDEBUG -Wall -Wextra -Ofast -Wno-unused-function -Wno-unused-variable

%/lib.o: cvector.c
	cc $(CFLAGS) -c $^ -o $@

%/c-slib: %/lib.o

tests: cvector_test cvector_fatpointer_test

test/cvector_test.o: test/cvector_test.c
	$(CC) $(CFLAGS) -o $@ -c $^

cvector_test: lib.o test/cvector_test.o
	$(CC) -lcunit -o test/$@ $^

test/cvector_fatpointer_test.o: test/cvector_fatpointer_test.c
	$(CC) $(CFLAGS) -o $@ -c $^

cvector_fatpointer_test: test/cvector_fatpointer_test.o
	$(CC) -lcunit -o test/$@ $^

tests: cvector_test cvector_fatpointer_test

clean:
	@for f in $$(find test/ -name "*.o")\
		$$(find test/ -type f -executable) ; \
		do rm $$f ; \
	done
