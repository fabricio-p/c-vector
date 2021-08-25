CFLAGS += -I ./ -std=c99 -DDEBUG -Wall -Wextra -O0 -g

%/cvector.o: cvector.c
	cc $(CFLAGS) -c $^ -o $@

vector.o: ./vector.o

test: cvector.o
	$(CC) $(CFLAGS) -o test/cvector_test.o -c test/cvector_test.c
	$(CC) -lcunit -o test/cvector_test $^ test/cvector_test.o
