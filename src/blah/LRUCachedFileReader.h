#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct LRUCachedFile LRUCachedFile;

typedef struct LRUCachedFileReader {
    LRUCachedFile* LRUCachedFile;
    int ReadPosition;
} LRUCachedFileReader;

bool LRUCachedFileReader_init(LRUCachedFile* lruCachedFile, LRUCachedFileReader* lruCachedFileReader);

int LRUCachedFileReader_getPosition(LRUCachedFileReader* lruCachedFileReader);

void LRUCachedFileReader_setPosition(LRUCachedFileReader* lruCachedFileReader, int position);

void LRUCachedFileReader_skipAhead(LRUCachedFileReader* lruCachedFileReader, int offset);

bool LRUCachedFileReader_readU16BigEndian(LRUCachedFileReader* lruCachedFileReader, uint16_t* value);

bool LRUCachedFileReader_readU32BigEndian(LRUCachedFileReader* lruCachedFileReader, uint32_t* value);

bool LRUCachedFileReader_readBytes(LRUCachedFileReader* lruCachedFileReader, uint8_t* buffer, int count);

#ifdef __cplusplus
}
#endif
