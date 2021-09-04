#include "testing.h"
#define CVECTOR_FATPOINTER
#include "cvector.h"

CVECTOR_WITH_NAME(char, StringBuffer);

void test_StringBuffer_new(void) {
	StringBuffer str = StringBuffer_new();
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_len(str), 0);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_cap(str), 0);
	StringBuffer_cleanup(str);
}
void test_StringBuffer_with_capacity(void) {
	StringBuffer str = StringBuffer_with_capacity(8);
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_len(str), 0);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_cap(str), 8);
	StringBuffer_cleanup(str);
}
void test_StringBuffer_with_length(void) {
	StringBuffer str = StringBuffer_with_length(4);
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_len(str), 4);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_cap(str), 4);
	StringBuffer_cleanup(str);
}
void test_StringBuffer_with_fill(void) {
	StringBuffer str = StringBuffer_with_fill(4, 'm');
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_len(str), 4);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_cap(str), 4);
	for (int i = 0; i < StringBuffer_len(str); i++) {
		CU_ASSERT_EQUAL_FATAL(str[i], 'm');
	}
	StringBuffer_cleanup(str);
}
void test_StringBuffer_at(void) {
	StringBuffer str;
	str = StringBuffer_new();
	CU_ASSERT_PTR_NULL_FATAL(StringBuffer_at(str, 0));
	StringBuffer_cleanup(str);
	str = StringBuffer_with_fill(1, 'z');
	CU_ASSERT_PTR_EQUAL_FATAL(StringBuffer_at(str, 0), (void *)str);
	CU_ASSERT_EQUAL_FATAL(*StringBuffer_at(str, 0), 'z');
	StringBuffer_cleanup(str);
}
void test_StringBuffer_push(void) {
	StringBuffer str = StringBuffer_with_capacity(3);
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_push(&str, 'f'), 0);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_push(&str, 'o'), 0);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_push(&str, 'o'), 0);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_len(str), 3);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_cap(str), 3);
	
	CU_ASSERT_FATAL(memcmp(str, "foo", 3) == 0);
	StringBuffer_cleanup(str);
}
void test_StringBuffer_pop(void) {
	StringBuffer str = StringBuffer_with_fill(2, 'y');
	CU_ASSERT_PTR_NOT_NULL_FATAL(str);
	CU_ASSERT_EQUAL_FATAL(StringBuffer_pop(&str), 'y');
	CU_ASSERT_EQUAL_FATAL(StringBuffer_pop(&str), 'y');
	StringBuffer_cleanup(str);
}

int main(int argc, char **argv) {
	int status = 0;
	CU_initialize_registry();
	CU_TestInfo StringBuffer_tests[] = {
		{ "new",           test_StringBuffer_new           },
		{ "with_capacity", test_StringBuffer_with_capacity },
		{ "with_length",   test_StringBuffer_with_length   },
		{ "with_fill",     test_StringBuffer_with_fill     },
		{ "at",            test_StringBuffer_at            },
		{ "push",          test_StringBuffer_push          },
		{ "pop",           test_StringBuffer_pop           },
		CU_TEST_INFO_NULL
	};
	CU_SuiteInfo suites[] = {
		{ "cvector<fatpointer>/StringBuffer",
		  NULL, NULL, NULL, NULL, StringBuffer_tests },
		CU_SUITE_INFO_NULL
	};
	if ((status = CU_register_suites(suites)) != CUE_SUCCESS)
		goto cleanup;
	RUN_TESTS;
cleanup:
	CU_cleanup_registry();
	return status;
}
