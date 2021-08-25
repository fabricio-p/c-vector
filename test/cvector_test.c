#include <cvector.h>
#include <sys/random.h>
#include "testing.h"

CVECTOR_DECLARE_WITH_NAME(int, IntVector);
CVECTOR_WITH_NAME(int, IntVector);

static IntVector vec;
void print_int(int *ptr) { printf("%d", *ptr); }
void test_push(void) {
				IntVector_cleanup(&vec);
				CU_ASSERT_EQUAL_FATAL(IntVector_init(&vec), 0);
				vec.print_item = print_int;

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
void test_set(void) {
				IntVector_cleanup(&vec);
				CU_ASSERT_EQUAL_FATAL(IntVector_init_with_fill(&vec, 4, 0), 0);
				vec.print_item = print_int;
				int ints[4];
				for (int i = 0; i < 4; i++)  {
								int val = ints[i] = rand();
								CU_ASSERT_EQUAL_FATAL(
												IntVector_set(&vec, i, val),
												0
								);
				}
				for (int i = 0; i < 4; i++) {
								int *ptr = (void *)vec.data + i * sizeof(int);
								CU_ASSERT_EQUAL_FATAL(*ptr, ints[i]);
				}
}
void test_pop(void) {
				IntVector_cleanup(&vec);
				CU_ASSERT_EQUAL_FATAL(IntVector_init_with_fill(&vec, 4, 132), 0);
				vec.print_item = print_int;

				while (vec.len) {
								CU_ASSERT_EQUAL_FATAL(IntVector_pop(&vec), 132);
				}
}
void test_get(void) {
				IntVector_cleanup(&vec);
				CU_ASSERT_EQUAL_FATAL(IntVector_init_with_fill(&vec, 2, 0), 0);
				vec.print_item = print_int;

				IntVector_set(&vec, 0, 7465);
				IntVector_set(&vec, 1, 13237);

				CU_ASSERT_PTR_EQUAL_FATAL(IntVector_get(&vec, 0), vec.data);
				CU_ASSERT_PTR_EQUAL_FATAL(IntVector_get(&vec, 1),
																  vec.data + sizeof(int));

				CU_ASSERT_EQUAL_FATAL(*IntVector_get(&vec, 0), 7465);
				CU_ASSERT_EQUAL_FATAL(*IntVector_get(&vec, 1), 13237);
}

int main(int argc, char **argv) {
				int status = 0;
				CU_initialize_registry();
				CU_TestInfo cvector_tests[] = {
								{ "push", test_push },
								{ "set",  test_set  },
								{ "pop",  test_pop  },
								{ "get",  test_get  },
								CU_TEST_INFO_NULL
				};
				CU_SuiteInfo cvector_suites[] = {
								{ "cvector_t/IntVector", NULL, NULL, NULL, NULL,
												cvector_tests },
								CU_SUITE_INFO_NULL
				};
				if ((status = CU_register_suites(cvector_suites)) != CUE_SUCCESS)
								goto cleanup;
				RUN_TESTS;
cleanup:
				CU_cleanup_registry();
				return status;
}
