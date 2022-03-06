#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct LinearAllocator LinearAllocator;
typedef struct LRUCachedFile LRUCachedFile;
typedef struct TestDescriptor TestDescriptor;

bool HunkFileParser_findTests(LRUCachedFile* lruCachedFile, LinearAllocator* linearAllocator, int* numTests, TestDescriptor** tests);

#ifdef __cplusplus
}
#endif
