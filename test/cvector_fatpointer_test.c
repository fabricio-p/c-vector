#include "testing.h"
#define CVECTOR_FATPOINTER
#include "lib.h"

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

#define CVECTOR_NO_TYPEDEF
#include "lib.h"
typedef struct Rocket  Rocket;
typedef struct Rocket* Rockets;
typedef struct Stage   Stage;
typedef struct Stage*  Stages;
typedef struct Engine  Engine;
typedef struct Engine* Engines;

struct Rocket {
	char   *name;
	int    weight_kg;
	Stages stages;
};
struct Stage {
	int     nth;
	Engines engines;
	int     weight_kg;
	int     fuel_weight;
};
struct Engine {
	char   *name;
	float  isp;
	int    weight;
	float  f2o_r[2];
};
CVECTOR_WITH_NAME(Rocket, Rockets);
CVECTOR_WITH_NAME(Stage, Stages);
CVECTOR_WITH_NAME(Engine, Engines);

void test_Rockets_new(void) {
	Rockets rockets = Rockets_new();
	CU_ASSERT_PTR_NOT_NULL_FATAL(rockets);
	Rockets_cleanup(rockets);
}
void test_Rockets_with_capacity(void) {
	Rockets rockets = Rockets_with_capacity(4);
	CU_ASSERT_PTR_NOT_NULL_FATAL(rockets);
	CU_ASSERT_EQUAL_FATAL(Rockets_len(rockets), 0);
	CU_ASSERT_EQUAL_FATAL(Rockets_cap(rockets), 4);
	Rockets_cleanup(rockets);
}
void test_Rockets_with_length(void) {
	Rockets rockets = Rockets_with_length(5);
	CU_ASSERT_PTR_NOT_NULL_FATAL(rockets);
	CU_ASSERT_EQUAL_FATAL(Rockets_len(rockets), 5);
	CU_ASSERT_EQUAL_FATAL(Rockets_cap(rockets), 5);
	Rockets_cleanup(rockets);
}
void test_Rockets_with_fill(void) {
	Rocket base_rocket = {
		.name = "Falcon Heavy",
		.weight_kg = 42000, // don't really know
	};
	Rockets rockets = Rockets_with_fill(1, base_rocket);
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
	CU_TestInfo Rockets_tests[] = {
		{ "new",           test_Rockets_new           },
		{ "with_capacity", test_Rockets_with_capacity },
		{ "with_length",   test_Rockets_with_length   },
		CU_TEST_INFO_NULL
	};
	CU_SuiteInfo suites[] = {
		{ "cvector<fatpointer>/StringBuffer",
		  NULL, NULL, NULL, NULL, StringBuffer_tests },
		{ "cvector<fatpointer>/Rockets",
			NULL, NULL, NULL, NULL, Rockets_tests },
		CU_SUITE_INFO_NULL
	};
	if ((status = CU_register_suites(suites)) != CUE_SUCCESS)
		goto cleanup;
	RUN_TESTS;
cleanup:
	CU_cleanup_registry();
	return status;
}
