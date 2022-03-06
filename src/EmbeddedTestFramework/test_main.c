#include "EmbeddedTestRunner.h"
#include "HunkFileParser.h"
#include "LinearAllocator.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "test_main.h"
#include "TestDescriptor.h"
#include "TestResult.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool getTests(const char* fileName, LinearAllocator* linearAllocator, int* numTests, TestDescriptor** tests)
{
    log_debug("Parsing file '%s'", fileName);

    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    if (!HunkFileParser_findTests(&lruCachedFile, linearAllocator, numTests, tests)) {
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    LRUCachedFile_close(&lruCachedFile);

    return true;
}

void printTests(int numTests, TestDescriptor* tests)
{
    printf("%d tests found in executable\n", numTests);
    for (int testId = 0; testId < numTests; testId++) {
        TestDescriptor* test = &tests[testId];
        printf("Test: %s, Hunk: %d, Offset: %d\n", test->Name, test->Hunk, test->Offset);
    }
}

bool listTests(const char* fileName, LinearAllocator* linearAllocator)
{
    int numTests;
    TestDescriptor* tests;
    if (!getTests(fileName, linearAllocator, &numTests, &tests)) {
        return false;
    }

    printTests(numTests, tests);
    return true;
}

bool runTests(const char* fileName, LinearAllocator* linearAllocator)
{
    int numTests;
    TestDescriptor* tests;
    if (!getTests(fileName, linearAllocator, &numTests, &tests)) {
        return false;
    }

    printf("%d tests found in executable\n", numTests);

    TestResult* testResults;

    if (!EmbeddedTestRunner_runTests(linearAllocator, numTests, tests, &testResults)) {
        return false;
    }

    switch (EmbeddedTestRunner_analyzeResults(numTests, testResults)) {
        case TestResultState_Pass:
            printf("All tests passed\n");
            return true;
        case TestResultState_Fail:
            printf("Some tests failed\n");
            return false;
        case TestResultState_Unknown:
            printf("Inconclusive results\n");
            return false;
    }
}

int test_main(int argc, char** argv)
{
    log_set_level(LOG_INFO);

    LinearAllocator linearAllocator;
    static uint8_t buffer[65536];
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    if ((argc >= 2) && !strcmp(argv[1], "list")) {
        if (!listTests(argv[0], &linearAllocator))
            return 1;
    } else if ((argc >= 2) && !strcmp(argv[1], "run")) {
        if (!runTests(argv[0], &linearAllocator))
            return 1;
    } else {
        printf("Usage: %s <command>\n", argv[0]);
        printf("Commands: \n");
        printf("\tlist\t - lists available tests\n");
        printf("\trun\t - runs all tests\n");
        return 1;
    }

    return 0;
}
