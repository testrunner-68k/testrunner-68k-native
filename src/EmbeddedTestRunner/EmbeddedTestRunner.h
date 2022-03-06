#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct Test;

typedef void (*TestEntryPoint)(void);

TestEntryPoint EmbeddedTestRunner_getTestEntryPoint(struct Test* test);
bool EmbeddedTestRunner_runTest(struct Test* test);
bool EmbeddedTestRunner_runTests(int numTests, struct Test* tests);

#ifdef __cplusplus
}
#endif
