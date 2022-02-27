
#include "Endian.h"
#include "LRUCachedFileReader.h"

bool LRUCachedFileReader_init(LRUCachedFile* lruCachedFile, LRUCachedFileReader* lruCachedFileReader)
{
    lruCachedFileReader->LRUCachedFile = lruCachedFile;
    lruCachedFileReader->ReadPosition = 0;
}

int LRUCachedFileReader_getPosition(LRUCachedFileReader* lruCachedFileReader)
{
    return lruCachedFileReader->ReadPosition;
}

void LRUCachedFileReader_setPosition(LRUCachedFileReader* lruCachedFileReader, int position)
{
    lruCachedFileReader->ReadPosition = position;
}

void LRUCachedFileReader_skipAhead(LRUCachedFileReader* lruCachedFileReader, int offset)
{
    lruCachedFileReader->ReadPosition += offset;
}

bool LRUCachedFileReader_readU32BigEndian(LRUCachedFileReader* lruCachedFileReader, uint32_t* value)
{
    union
    {
        uint8_t u8[4];
        uint32_t u32;
    } u8_and_u32 = { .u8 = { 0x12, 0x34, 0x56, 0x78 } };

    if (!LRUCachedFile_readAt(lruCachedFileReader->LRUCachedFile, u8_and_u32.u8, lruCachedFileReader->ReadPosition, sizeof(uint32_t)))
        return false;

    *value = Endian_readU32BigEndian(&u8_and_u32.u32);

    lruCachedFileReader->ReadPosition += sizeof(uint32_t);
    return true;
}
