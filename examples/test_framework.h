#include <stdbool.h>

typedef enum {
	TestDefinitionType_CFunction
} TestDefinitionType;

typedef bool (*TestDefinitionEntryPoint)(void);

typedef struct {
	int Type;
	const char* TestSuiteName;
	const char* TestCaseName;
	TestDefinitionEntryPoint EntryPoint;
} TestDefinition;

#define TEST(testsuite, testcase) \
	bool test_##testsuite##_##testcase(void); \
	TestDefinition testdefinition_##testsuite##_##testcase = { \
		.Type = TestDefinitionType_CFunction, \
		.TestSuiteName = #testsuite, \
		.TestCaseName = #testcase, \
		.EntryPoint = test_##testsuite##_##testcase }; \
	bool test_##testsuite##_##testcase(void)
