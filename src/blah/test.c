#include "HunkFileParser.h"
#include "LinearAllocator.h"
#include "log.h"
#include "LRUCachedFile.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool readThingy(const char* fileName)
{
    log_debug("Parsing file '%s'", fileName);

    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    LinearAllocator linearAllocator;
    uint8_t buffer[65536];
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    int numTests;
    Test* tests;
    if (!HunkFileParser_findTests(&lruCachedFile, &linearAllocator, &numTests, &tests)) {
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    for (int testId = 0; testId < numTests; testId++) {
        Test* test = &tests[testId];
        log_info("Test: %s, Hunk: %d, Offset: %d", test->Name, test->Hunk, test->Offset);
    }

    LRUCachedFile_close(&lruCachedFile);

    return true;
}

int main(int argc, const char** argv)
{

    if (argc < 2)
    {
        printf("Usage: %s <amiga executable>\n\n", argv[0]);
        return 0;
    }

    if (!readThingy(argv[1]))
        return 1;

    return 0;
}
