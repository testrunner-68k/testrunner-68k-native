#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LRUCachedFile.h"
#include "log.h"

LRUCachedFile* LRUCachedFile_open(const char* fileName)
{
    LRUCachedFile* lruCachedFile = (LRUCachedFile*)malloc(sizeof(LRUCachedFile));

    if (!(lruCachedFile->File = fopen(fileName, "r"))) {
        log_error("Unable to open '%s'", fileName);
        free(lruCachedFile);
        return 0;
    }

    if (fseek(lruCachedFile->File, 0, SEEK_END)) {
        log_error("Error while seeking to end of '%s'", fileName);
        free(lruCachedFile);
        return 0;
    }
    lruCachedFile->FileSize = ftell(lruCachedFile->File);

    for (int cachedPageIndex = 0; cachedPageIndex < LRUCachedFile_PageCount; cachedPageIndex++) {
        LRUCachedFilePage* cachedPage = &lruCachedFile->CachedPages[cachedPageIndex];
        cachedPage->PageIndex = -1;
        cachedPage->SequenceId = 0;
    }

    lruCachedFile->SequenceId = 0;

    return lruCachedFile;
}

void LRUCachedFile_close(LRUCachedFile* lruCachedFile)
{
    fclose(lruCachedFile->File);
    free(lruCachedFile);
}

LRUCachedFilePage* LRUCachedFile_cachePage(LRUCachedFile* lruCachedFile, int pageIndex)
{
    lruCachedFile->SequenceId++;

    log_debug("Caching page index %d", pageIndex);

    // Scan through cached pages, looking for a hit
    for (int cachedPageIndex = 0; cachedPageIndex < LRUCachedFile_PageCount; cachedPageIndex++) {
        LRUCachedFilePage* cachedPage = &lruCachedFile->CachedPages[cachedPageIndex];
        if (cachedPage->PageIndex == pageIndex) {
            // Cache hit: update last-use counter for page
            log_debug("Cache entry %d is a cache hit for page index %d; re-using existing cache entry", cachedPageIndex, pageIndex);
            cachedPage->SequenceId = lruCachedFile->SequenceId;
            return cachedPage;
        }
    }


    // No hit; locate unused or least-recently-used page

    int cachedPageToReuseIndex = 0;
    int cachedPageToReuseSequenceId = 0x7fffffff;

    for (int cachedPageIndex = 0; cachedPageIndex < LRUCachedFile_PageCount; cachedPageIndex++) {
        LRUCachedFilePage* cachedPage = &lruCachedFile->CachedPages[cachedPageIndex];

        // Cache entry is unused; use this
        if (cachedPage->PageIndex == -1) {
            log_debug("Cache entry %d is unused; allocating this entry to cache page index %d", cachedPageIndex, pageIndex);
            cachedPageToReuseIndex = cachedPageIndex;
            break;
        }

        // Current cache entry is least recently used out of entries inspected so far;
        //  select it as LRU eviction candidate, but keep scanning
        if (cachedPage->SequenceId < cachedPageToReuseSequenceId) {
            cachedPageToReuseIndex = cachedPageIndex;
            cachedPageToReuseSequenceId = cachedPage->SequenceId;
        }
    }

    log_debug("Using cache entry %d to cache page index %d", cachedPageToReuseIndex, pageIndex);

    // Re-use selected cache entry for the new page

    const int pageOffsetStart = pageIndex * LRUCachedFile_PageSize;
    int pageOffsetEnd = (pageIndex + 1) * LRUCachedFile_PageSize;
    pageOffsetEnd = (pageOffsetEnd <= lruCachedFile->FileSize ? pageOffsetEnd : lruCachedFile->FileSize);
    const int pageSize = pageOffsetEnd - pageOffsetStart;

    LRUCachedFilePage* cachedPage = &lruCachedFile->CachedPages[cachedPageToReuseIndex];
    cachedPage->PageIndex = pageIndex;
    cachedPage->PageSize = pageSize;
    cachedPage->SequenceId = lruCachedFile->SequenceId;

    if (fseek(lruCachedFile->File, cachedPage->PageIndex * LRUCachedFile_PageSize, SEEK_SET)) {
        log_error("Error while seeking in file");
        return 0;
    }

    if (fread(cachedPage->Data, 1, cachedPage->PageSize, lruCachedFile->File) != cachedPage->PageSize) {
        log_error("Unable to read entire page from file");
        return 0;
    }

    return cachedPage;
}

bool LRUCachedFile_readAt(LRUCachedFile* lruCachedFile, uint8_t* buffer, int offset, int count)
{
    const int startOffset = offset;
    const int endOffset = offset + count;

    log_debug("Reading from cached file; offset = %d, count = %d", offset, count);

    if ((startOffset < 0)
        || (startOffset > lruCachedFile->FileSize)
        || (endOffset > lruCachedFile->FileSize)) {
        log_error("Cannot read outside file boundaries; file size = %d, offset = %d, count = %d", lruCachedFile->FileSize, offset, count);
        return false;
    }

    const int firstPageIndex = (offset / LRUCachedFile_PageSize);
    const int lastPageIndex = ((offset + count + LRUCachedFile_PageSize - 1) / LRUCachedFile_PageSize) - 1;

    log_debug("The section to read covers pages [%d, %d[", firstPageIndex, lastPageIndex);

    uint8_t* writePtr = buffer;

    for (int pageIndex = firstPageIndex; pageIndex <= lastPageIndex; pageIndex++) {
        LRUCachedFilePage* cachedPage = LRUCachedFile_cachePage(lruCachedFile, pageIndex);
        if (!cachedPage)
            return false;

        const int cachedPageStartOffset = (pageIndex == firstPageIndex ? startOffset % LRUCachedFile_PageSize : 0);
        const int cachedPageEndOffset = (pageIndex == lastPageIndex ? ((endOffset + (LRUCachedFile_PageSize - 1)) % LRUCachedFile_PageSize) + 1 : LRUCachedFile_PageSize);
        const int cachedPageCopySize = cachedPageEndOffset - cachedPageStartOffset;

        memcpy(writePtr, &cachedPage->Data[cachedPageStartOffset], cachedPageCopySize);
        writePtr += cachedPageCopySize;
    }

    log_debug("ReadAt complete");
    return true;
}