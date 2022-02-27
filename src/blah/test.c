#include "HunkFileParser.h"
#include "log.h"
#include "LRUCachedFile.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool readThingy(const char* fileName)
{
    log_debug("Parsing file '%s'", fileName);

    LRUCachedFile lruCachedFile;
    if (!LRUCachedFile_open(&lruCachedFile, fileName)) {
        return false;
    }

    if (!HunkFileParser_findTests(&lruCachedFile)) {
        LRUCachedFile_close(&lruCachedFile);
        return false;
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
