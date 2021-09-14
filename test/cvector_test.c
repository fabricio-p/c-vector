#include "lib.h"
#include <sys/random.h>
#include <string.h>
#include "testing.h"

CVECTOR_DECLARE_WITH_NAME(int, IntVector);
CVECTOR_WITH_NAME(int, IntVector);

static IntVector vec;
void print_int(int *ptr) { printf("%d", *ptr); }
void test_push(void) {
	IntVector_cleanup(&vec);
	CU_ASSERT_EQUAL_FATAL(IntVector_init(&vec), 0);
	vec.print_item = (void (*)(void *))print_int;

	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 8), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 45), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 244), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 3543), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 25414), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 341123), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 1312312), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 13224443), 0);
	
	CU_ASSERT_EQUAL_FATAL(vec.item_size, sizeof(int));
	CU_ASSERT_EQUAL_FATAL(vec.len, 8);
	CU_ASSERT_EQUAL_FATAL(vec.cap, 8);
	CU_ASSERT_PTR_NOT_NULL_FATAL(vec.data);
}
void test_set_at(void) {
	IntVector_cleanup(&vec);
	CU_ASSERT_EQUAL_FATAL(IntVector_init_with_length(&vec, 4), 0);
	vec.print_item = (void (*)(void *))print_int;
	int ints[4];
	for (int i = 0; i < 4; i++)  {
		int val = ints[i] = rand();

		CU_ASSERT_EQUAL_FATAL(IntVector_set_at(&vec, i, val), 0);
	}
	for (int i = 0; i < 4; i++) {
		int *ptr = (void *)vec.data + i * sizeof(int);
		CU_ASSERT_EQUAL_FATAL(*ptr, ints[i]);
	}
}
void test_pop(void) {
	IntVector_cleanup(&vec);
	CU_ASSERT_EQUAL_FATAL(IntVector_init_with_fill(&vec, 4, 132), 0);
	vec.print_item = (void (*)(void *))print_int;

	while (vec.len) {
		CU_ASSERT_EQUAL_FATAL(IntVector_pop(&vec), 132);
	}
}
void test_at(void) {
	IntVector_cleanup(&vec);
	CU_ASSERT_EQUAL_FATAL(IntVector_init_with_length(&vec, 2), 0);
	vec.print_item = (void (*)(void *))print_int;

	IntVector_set(&vec, 0, 7465);
	IntVector_set(&vec, 1, 13237);

	CU_ASSERT_PTR_EQUAL_FATAL(IntVector_at(&vec, 0), vec.data);
	CU_ASSERT_PTR_EQUAL_FATAL(IntVector_at(&vec, 1),
	  vec.data + sizeof(int));

	CU_ASSERT_EQUAL_FATAL(*IntVector_at(&vec, 0), 7465);
	CU_ASSERT_EQUAL_FATAL(*IntVector_at(&vec, 1), 13237);
}

void test_clone(void) {
	IntVector_cleanup(&vec);
	CU_ASSERT_EQUAL_FATAL(IntVector_init_with_capacity(&vec, 6), 0);
	vec.print_item = (void (*)(void *))print_int;

	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 24), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 354), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, -343), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, -244), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, 35553), 0);
	CU_ASSERT_EQUAL_FATAL(IntVector_push(&vec, -35345), 0);

	IntVector new_vec = IntVector_clone(&vec);

	CU_ASSERT_EQUAL_FATAL(new_vec.len, vec.len);
	CU_ASSERT_EQUAL_FATAL(new_vec.cap, vec.cap);
	CU_ASSERT_EQUAL_FATAL(new_vec.size, vec.size);

	for (int i = 0; i < new_vec.len; i++) {
		CU_ASSERT_EQUAL_FATAL(*IntVector_get(&new_vec, i),
							  *IntVector_get(&vec, i));
	}
}

typedef struct {
	char *name;
	int  age;
} Person;

CVECTOR_DECLARE_WITH_NAME(Person, People);
CVECTOR_WITH_NAME(Person, People);

Person clone_person(Person *person) {
	Person new_person;
	new_person.name = strdup(person->name);
	new_person.age = person->age;
	return new_person;
}

void test_deep_clone(void) {
	People people;
	CU_ASSERT_EQUAL_FATAL(People_init_with_capacity(&people, 4), 0);

	Person p1 = { strdup("Ben"),      10 },
		   p2 = { strdup("Fabricio"), 15 },
		   p3 = { strdup("amogus"),   -1 },
		   p4 = { strdup("Bob"),      40 };

	CU_ASSERT_EQUAL_FATAL(People_push(&people, p1), 0);
	CU_ASSERT_EQUAL_FATAL(People_push(&people, p2), 0);
	CU_ASSERT_EQUAL_FATAL(People_push(&people, p3), 0);
	CU_ASSERT_EQUAL_FATAL(People_push(&people, p4), 0);

	People people_clone = People_deep_clone(&people, clone_person);

	CU_ASSERT_EQUAL_FATAL(people_clone.len, people.len);
	CU_ASSERT_EQUAL_FATAL(people_clone.cap, people.cap);
	CU_ASSERT_EQUAL_FATAL(people_clone.size, people.size);
	CU_ASSERT_PTR_NOT_EQUAL(people_clone.data, people.data);

	for (int i = 0; i < people.len; i++) {
		Person *person = People_get(&people, i);
		Person *person_clone = People_get(&people_clone, i);
		CU_ASSERT_PTR_NOT_NULL_FATAL(person);
		CU_ASSERT_PTR_NOT_NULL_FATAL(person_clone);

		CU_ASSERT_PTR_NOT_EQUAL(person->name, person_clone->name);
		CU_ASSERT_EQUAL_FATAL(person->age, person_clone->age);
		CU_ASSERT_STRING_EQUAL_FATAL(person->name, person_clone->name);
		free(person->name);
		free(person_clone->name);
	}

	People_cleanup(&people);
	People_cleanup(&people_clone);
}

int main(int argc, char **argv) {
	int status = 0;
	CU_initialize_registry();
	CU_TestInfo IntVector_tests[] = {
		{ "push",   test_push   },
		{ "set_at", test_set_at },
		{ "pop",    test_pop    },
		{ "at",	    test_at     },
		{ "clone",  test_clone  },
		CU_TEST_INFO_NULL
	};
	CU_TestInfo People_tests[] = {
		{ "deep_clone", test_deep_clone },
		CU_TEST_INFO_NULL
	};
	CU_SuiteInfo cvector_suites[] = {
		{ "cvector_t/IntVector", NULL, NULL, NULL, NULL,
			IntVector_tests },
		{ "cvector/People", NULL, NULL, NULL, NULL, People_tests },
		CU_SUITE_INFO_NULL
	};
	if ((status = CU_register_suites(cvector_suites)) != CUE_SUCCESS)
		goto cleanup;
	RUN_TESTS;
cleanup:
	CU_cleanup_registry();
	return status;
}
