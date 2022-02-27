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

bool skipCodeDataDebug(LRUCachedFileReader* lruCachedFileReader) {
    uint32_t hunkSizeLongs;
    if (!LRUCachedFileReader_readU32BigEndian(lruCachedFileReader, &hunkSizeLongs)) {
        log_error("Error while reading hunk type");
        return false;
    }
    LRUCachedFileReader_skipAhead(lruCachedFileReader, hunkSizeLongs * sizeof(uint32_t));
    return true;
}

bool skipBss(LRUCachedFileReader* lruCachedFileReader) {
    LRUCachedFileReader_skipAhead(lruCachedFileReader, sizeof(uint32_t));
    return true;
}

bool skipReloc32(LRUCachedFileReader* lruCachedFileReader) {
    while (true) {
        uint32_t numOffsets;
        if (!LRUCachedFileReader_readU32BigEndian(lruCachedFileReader, &numOffsets)) {
            log_error("Error while reading number of hunk offsets");
            return false;
        }
        if (!numOffsets)
            return true;

        LRUCachedFileReader_skipAhead(lruCachedFileReader, (1 + numOffsets) * sizeof(uint32_t));
    }
}

bool skipDreloc32(LRUCachedFileReader* lruCachedFileReader) {
    while (true) {
        uint16_t numOffsets;
        if (!LRUCachedFileReader_readU16BigEndian(lruCachedFileReader, &numOffsets)) {
            log_error("Error while reading number of hunk offsets");
            return false;
        }
        if (!numOffsets)
            return true;

        LRUCachedFileReader_skipAhead(lruCachedFileReader, (1 + numOffsets) * sizeof(uint16_t));
        if (LRUCachedFileReader_getPosition(lruCachedFileReader) & 2)
            LRUCachedFileReader_skipAhead(lruCachedFileReader, sizeof(uint16_t));
    }
}

bool parseSymbols(LRUCachedFileReader* lruCachedFileReader) {
    log_error("parseSymbols is not yet implemented");
    return false;
}

bool findTests(LRUCachedFile* lruCachedFile, int headerSize) {

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(lruCachedFile, &lruCachedFileReader);
    LRUCachedFileReader_skipAhead(&lruCachedFileReader, headerSize);

    while (true) {

        uint32_t hunkTypeWithFlags;
        if (!LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &hunkTypeWithFlags)) {
            log_error("Error while reading hunk type");
            return false;
        }

        const uint32_t hunkType = hunkTypeWithFlags & 0x0fffffff;

        log_debug("Encountered hunk type: %08X", hunkType);

        switch (hunkType)
        {
            case HUNK_END:
                return true;

			case HUNK_SYMBOL: if (!parseSymbols(&lruCachedFileReader)) { return false; } break;

			case HUNK_DEBUG:
			case HUNK_CODE: 
			case HUNK_DATA: if (!skipCodeDataDebug(&lruCachedFileReader)) { return false; } break;

			case HUNK_BSS: if (!skipBss(&lruCachedFileReader)) { return false; } break;

			case HUNK_RELOC32: if (!skipReloc32(&lruCachedFileReader)) { return false; } break;

			case HUNK_DREL32:
			case HUNK_RELOC32SHORT: if (!skipDreloc32(&lruCachedFileReader)) { return false; } break;

			case HUNK_UNIT:
			case HUNK_NAME:
			case HUNK_RELOC16:
			case HUNK_RELOC8:
			case HUNK_EXT:
			case HUNK_HEADER:
			case HUNK_OVERLAY:
			case HUNK_BREAK:
			case HUNK_DREL16:
			case HUNK_DREL8:
			case HUNK_LIB:
			case HUNK_INDEX:
			case HUNK_RELRELOC32:
			case HUNK_ABSRELOC16:
			{
				log_error("Unsupported hunk type: %08X", hunkType);
                return false;
			}

			default:
			{
				log_error("Unknown hunk type: %08X", hunkType);
                return false;
			}
        }
    }
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

    if (!findTests(&lruCachedFile, hunkHeader.Size)) {
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
