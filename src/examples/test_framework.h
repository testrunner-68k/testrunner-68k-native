#include <stdbool.h>

#define TEST(testcase) \
	bool test____##testcase(void)

int test_main(int argc, char** argv);