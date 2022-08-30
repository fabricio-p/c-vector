#define CVECTOR_HEAP_STRUCT
#include "lib.h"
#include <sys/random.h>
#include <string.h>
#include "testing.h"

CVECTOR_WITH_NAME(int, IntVector);

static IntVector int_vec;
void test_push(void) {
  IntVector_cleanup(&int_vec);
  CU_ASSERT_EQUAL_FATAL(IntVector_init(&int_vec), 0);

  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 8), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 45), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 244), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 3543), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 25414), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 341123), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 1312312), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 13224443), 0);
  
  CU_ASSERT_EQUAL_FATAL(int_vec.l, 8);
  CU_ASSERT_EQUAL_FATAL(int_vec.c, 8);
  CU_ASSERT_PTR_NOT_NULL_FATAL(int_vec.d);
}
void test_set_at(void) {
  IntVector_cleanup(&int_vec);
  CU_ASSERT_EQUAL_FATAL(IntVector_init_with_length(&int_vec, 4), 0);
  int ints[4];
  for (int i = 0; i < 4; ++i)  {
    int val = ints[i] = rand();

    CU_ASSERT_EQUAL_FATAL(IntVector_set_at(&int_vec, i, val), 0);
  }
  for (int i = 0; i < 4; ++i) {
    int *ptr = (void *)int_vec.d + i * sizeof(int);
    CU_ASSERT_EQUAL_FATAL(*ptr, ints[i]);
  }
}
void test_pop_shrink_get(void) {
  IntVector_cleanup(&int_vec);
  CU_ASSERT_EQUAL_FATAL(IntVector_init_with_fill(&int_vec, 4, 132), 0);

  while (int_vec.l) {
    CU_ASSERT_EQUAL_FATAL(IntVector_pop_shrink_get(&int_vec), 132);
  }
}
void test_at(void) {
  IntVector_cleanup(&int_vec);
  CU_ASSERT_EQUAL_FATAL(IntVector_init_with_length(&int_vec, 2), 0);

  IntVector_set(&int_vec, 0, 7465);
  IntVector_set(&int_vec, 1, 13237);

  CU_ASSERT_PTR_EQUAL_FATAL(IntVector_at(&int_vec, 0), int_vec.d);
  CU_ASSERT_PTR_EQUAL_FATAL(IntVector_at(&int_vec, 1),
    int_vec.d + sizeof(int));

  CU_ASSERT_EQUAL_FATAL(*IntVector_at(&int_vec, 0), 7465);
  CU_ASSERT_EQUAL_FATAL(*IntVector_at(&int_vec, 1), 13237);
}

void test_clone(void) {
  IntVector_cleanup(&int_vec);
  CU_ASSERT_EQUAL_FATAL(IntVector_init_with_capacity(&int_vec, 6), 0);

  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 24), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 354), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, -343), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, -244), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, 35553), 0);
  CU_ASSERT_EQUAL_FATAL(IntVector_push(&int_vec, -35345), 0);

  IntVector new_int_vec = IntVector_clone(&int_vec);

  CU_ASSERT_EQUAL_FATAL(new_int_vec.l, int_vec.l);
  CU_ASSERT_EQUAL_FATAL(new_int_vec.c, int_vec.c);

  for (int i = 0; i < new_int_vec.l; ++i) {
    CU_ASSERT_EQUAL_FATAL(*IntVector_get(&new_int_vec, i),
                          *IntVector_get(&int_vec, i));
  }
}

typedef struct {
  char *name;
  int  age;
} Person;

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
       p2 = { strdup("Fabricio"), 16 },
       p3 = { strdup("amogus"),   -1 },
       p4 = { strdup("Bob"),      40 };

  CU_ASSERT_EQUAL_FATAL(People_push(&people, p1), 0);
  CU_ASSERT_EQUAL_FATAL(People_push(&people, p2), 0);
  CU_ASSERT_EQUAL_FATAL(People_push(&people, p3), 0);
  CU_ASSERT_EQUAL_FATAL(People_push(&people, p4), 0);

  People people_clone = People_deep_clone(&people, clone_person);

  CU_ASSERT_EQUAL_FATAL(people_clone.l, people.l);
  CU_ASSERT_EQUAL_FATAL(people_clone.c, people.c);
  CU_ASSERT_PTR_NOT_EQUAL(people_clone.d, people.d);

  for (int i = 0; i < people.l; ++i) {
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
    { "push",           test_push           },
    { "set_at",         test_set_at         },
    { "pop_shrink_get", test_pop_shrink_get },
    { "at",             test_at             },
    { "clone",          test_clone          },
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
