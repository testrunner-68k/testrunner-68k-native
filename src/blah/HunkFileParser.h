#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct LRUCachedFile LRUCachedFile;

bool HunkFileParser_findTests(LRUCachedFile* lruCachedFile);

#ifdef __cplusplus
}
#endif

