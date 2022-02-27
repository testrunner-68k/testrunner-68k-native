#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "LRUCachedFile.h"

typedef struct  {
    LRUCachedFile* LRUCachedFile;
    int ReadPosition;
} LRUCachedFileReader;

uint32_t Endian_ReadU32BigEndian(const uint32_t* value);

#ifdef __cplusplus
}
#endif
