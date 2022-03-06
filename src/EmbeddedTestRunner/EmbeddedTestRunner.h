#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct LinearAllocator LinearAllocator;
typedef struct TestDescriptor TestDescriptor;
typedef struct TestResult TestResult;
typedef enum TestResultState TestResultState;

typedef void (*TestEntryPoint)(void);

bool EmbeddedTestRunner_getLoadedSegments(LinearAllocator* linearAllocator, uint8_t*** loadedSegments, int* numLoadedSegments);
bool EmbeddedTestRunner_getTestEntryPoints(LinearAllocator* linearAllocator, int numTests, TestDescriptor* tests, TestEntryPoint** testEntryPoints);

bool EmbeddedTestRunner_runTest(TestDescriptor* test, TestEntryPoint testEntryPoint);
bool EmbeddedTestRunner_runTests(LinearAllocator* linearAllocator, int numTests, TestDescriptor* tests, TestResult** testResults);

TestResultState EmbeddedTestRunner_analyzeResults(int numTests, TestResult* testResults);

#ifdef __cplusplus
}
#endif
