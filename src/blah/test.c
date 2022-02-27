#include "doshunks.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "LRUCachedFileReader.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool readThingy(const char* fileName)
{
    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(&lruCachedFile, &lruCachedFileReader);

    uint32_t value;
    if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &value) || value != HUNK_HEADER) {
        log_error("File must begin with HUNK_HEADER");
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    log_info("yep!");

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
