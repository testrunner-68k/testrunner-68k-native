#include "doshunks.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "LRUCachedFileReader.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int Size;
} HunkHeader;

bool readHunkHeader(LRUCachedFile* lruCachedFile, HunkHeader* hunkHeader)
{
    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(lruCachedFile, &lruCachedFileReader);

    uint32_t value;
    if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &value) || value != HUNK_HEADER) {
        log_error("File must begin with HUNK_HEADER");
        return false;
    }

    log_debug("File has a valid HUNK_HEADER signature");

    log_debug("Skipping any resident library names...");

    while (true) {
        uint32_t stringLengthLongs;
        if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &stringLengthLongs)) {
            log_error("Error while reading string length");
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
        return false;
    }

    log_debug("Num hunk sizes: %d", numHunkSizes);
    log_debug("First load hunk: %d", firstLoadHunk);
    log_debug("Last load hunk: %d", lastLoadHunk);

    LRUCachedFileReader_skipAhead(&lruCachedFileReader, numHunkSizes * sizeof(uint32_t));

    hunkHeader->Size = LRUCachedFileReader_getPosition(&lruCachedFileReader);

    return true;
}

bool readThingy(const char* fileName)
{
    log_debug("Parsing file '%s'", fileName);

    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    HunkHeader hunkHeader;
    if (!readHunkHeader(&lruCachedFile, &hunkHeader)) {
        LRUCachedFile_close(&lruCachedFile);
        return false;
    }

    log_debug("Header hunk size: %d bytes", hunkHeader.Size);

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
