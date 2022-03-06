#include "EmbeddedTestRunner.h"
#include "HunkFileParser.h"
#include "LinearAllocator.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "test_main.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool getTests(const char* fileName, int* numTests, Test** tests)
{
    log_debug("Parsing file '%s'", fileName);

    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    LinearAllocator linearAllocator;
    uint8_t buffer[65536];
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    if (!HunkFileParser_findTests(&lruCachedFile, &linearAllocator, numTests, tests)) {
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    LRUCachedFile_close(&lruCachedFile);

    return true;
}

void printTests(int numTests, Test* tests)
{
    printf("%d tests found in executable\n", numTests);
    for (int testId = 0; testId < numTests; testId++) {
        Test* test = &tests[testId];
        printf("Test: %s, Hunk: %d, Offset: %d\n", test->Name, test->Hunk, test->Offset);
    }
}

bool listTests(const char* fileName)
{
    int numTests;
    Test* tests;
    if (!getTests(fileName, &numTests, &tests)) {
        return false;
    }

    printTests(numTests, tests);
    return true;
}

bool runTests(const char* fileName)
{
    int numTests;
    Test* tests;
    if (!getTests(fileName, &numTests, &tests)) {
        return false;
    }

    printf("%d tests found in executable\n", numTests);

    if (!EmbeddedTestRunner_runTests(numTests, tests)) {
        return false;
    }

    return true;
}

int test_main(int argc, char** argv)
{
    log_set_level(LOG_INFO);

    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        printf("Commands: \n");
        printf("\tlist\t - lists available tests\n");
        printf("\trun\t - runs all tests\n");
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        if (!listTests(argv[0]))
            return 1;
    } else if (!strcmp(argv[1], "run")) {
        if (!runTests(argv[0]))
            return 1;
    }

    return 0;
}
