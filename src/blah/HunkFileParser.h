#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct LinearAllocator LinearAllocator;
typedef struct LRUCachedFile LRUCachedFile;

typedef struct Test {
    const char* Name;
    int Hunk;
    int Offset;
} Test;

bool HunkFileParser_findTests(LRUCachedFile* lruCachedFile, LinearAllocator* linearAllocator, int* numTests, Test** tests);

#ifdef __cplusplus
}
#endif

