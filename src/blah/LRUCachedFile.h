#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum {
    LRUCachedFile_PageSize = 4096,
    LRUCachedFile_PageCount = 3
};

typedef struct {
    int PageIndex;
    int PageSize;
    int SequenceId;
    uint8_t Data[LRUCachedFile_PageSize];
} LRUCachedFilePage;

typedef struct {
    FILE* File;
    int FileSize;
    int SequenceId;
    LRUCachedFilePage CachedPages[LRUCachedFile_PageCount];
} LRUCachedFile;

bool LRUCachedFile_open(LRUCachedFile* lruCachedFile, const char* fileName);

void LRUCachedFile_close(LRUCachedFile* lruCachedFile);

bool LRUCachedFile_readAt(LRUCachedFile* lruCachedFile, uint8_t* buffer, int offset, int count);

#ifdef __cplusplus
}
#endif
