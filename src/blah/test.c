#include "doshunks.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "LRUCachedFileReader.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool readThingy(const char* fileName)
{
    log_debug("Parsing file '%s'", fileName);

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

    log_debug("File has a valid HUNK_HEADER signature");


    log_debug("Skipping any resident library names...");

    while (true) {
        uint32_t stringLengthLongs;
        if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &stringLengthLongs)) {
            log_error("Error while reading string length");
            LRUCachedFile_close(&lruCachedFile);
            return false;
        }

        // We have reached the end of resident library names list
        if (!stringLengthLongs)
            break;

        LRUCachedFileReader_skipAhead(&lruCachedFileReader, (int)stringLengthLongs * 4);
    }

    log_debug("Skipping any resident library names...");

    uint32_t numHunkSizes, firstLoadHunk, lastLoadHunk;
    if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &numHunkSizes)
        || !LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &firstLoadHunk)
        || !LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &lastLoadHunk)) {
        log_error("Error while reading hunk header");
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    log_debug("Num hunk sizes: %d", numHunkSizes);
    log_debug("First load hunk: %d", firstLoadHunk);
    log_debug("Last load hunk: %d", lastLoadHunk);

    for (int hunkIndex = 0; hunkIndex < (int)numHunkSizes; hunkIndex++) {
        uint32_t hunkSizeWithMemFlags;
        if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &hunkSizeWithMemFlags)) {
            log_error("Error while reading hunk header");
            LRUCachedFile_close(&lruCachedFile);
            return false;
        }

        log_debug("Hunk %d size: %d longs", hunkIndex, hunkSizeWithMemFlags & 0x3fffffff);
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
