#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "LRUCachedFile.h"

typedef struct  {
    LRUCachedFile* LRUCachedFile;
    int ReadPosition;
} LRUCachedFileReader;

bool LRUCachedFileReader_init(LRUCachedFile* lruCachedFile, LRUCachedFileReader* lruCachedFileReader);

int LRUCachedFileReader_getPosition(LRUCachedFileReader* lruCachedFileReader);

void LRUCachedFileReader_setPosition(LRUCachedFileReader* lruCachedFileReader, int position);

void LRUCachedFileReader_skipAhead(LRUCachedFileReader* lruCachedFileReader, int offset);

bool LRUCachedFileReader_readU32BigEndian(LRUCachedFileReader* lruCachedFileReader, uint32_t* value);

#ifdef __cplusplus
}
#endif
