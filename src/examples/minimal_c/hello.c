#include "../test_framework.h"

int main(int argc, char** argv)
{
	return test_main(argc, argv);
}

TEST(TestCase1) {
	return true;
}

TEST(TestCase2) {
	return false;
}

TEST(TestCase3) {
	return true;
}
