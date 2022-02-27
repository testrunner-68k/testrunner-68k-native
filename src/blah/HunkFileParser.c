#include "doshunks.h"
#include "log.h"
#include "LRUCachedFile.h"
#include "LRUCachedFileReader.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {
    MaxSymbolLength = 1024  // Max symbol length supported by this parser; longer symbols result in parse failure
};

typedef struct {
    int Size;
} HunkHeader;

bool HunkFileParser_readHunkHeader(LRUCachedFile* lruCachedFile, HunkHeader* hunkHeader)
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

bool HunkFileParser_skipCodeDataDebug(LRUCachedFileReader* lruCachedFileReader) {
    uint32_t hunkSizeLongs;
    if (!LRUCachedFileReader_readU32BigEndian(lruCachedFileReader, &hunkSizeLongs)) {
        log_error("Error while reading hunk type");
        return false;
    }
    LRUCachedFileReader_skipAhead(lruCachedFileReader, hunkSizeLongs * sizeof(uint32_t));
    return true;
}

bool HunkFileParser_skipBss(LRUCachedFileReader* lruCachedFileReader) {
    LRUCachedFileReader_skipAhead(lruCachedFileReader, sizeof(uint32_t));
    return true;
}

bool HunkFileParser_skipReloc32(LRUCachedFileReader* lruCachedFileReader) {
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

bool HunkFileParser_skipDreloc32(LRUCachedFileReader* lruCachedFileReader) {
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

const char* testCaseSymbolPrefix = "_test____";

const char* HunkFileParser_getTestCaseNameFromSymbol(const char* symbol) {
    int testCaseSymbolPrefixLength = strlen(testCaseSymbolPrefix);
    if (memcmp(symbol, testCaseSymbolPrefix, testCaseSymbolPrefixLength))
        return 0;
    else
        return symbol + testCaseSymbolPrefixLength;
}

bool HunkFileParser_parseSymbols(LRUCachedFileReader* lruCachedFileReader, int hunkId) {

    log_debug("Parsing symbols for hunk %d", hunkId);

    static char symbolBuffer[MaxSymbolLength + 1];

    while (true) {
        uint32_t symbolLengthLongs;
        if (!LRUCachedFileReader_readU32BigEndian(lruCachedFileReader, &symbolLengthLongs)) {
            log_error("Error while reading symbol length");
            return false;
        }
        if (!symbolLengthLongs)
            return true;

        const uint32_t symbolLengthBytes = symbolLengthLongs * 4;

        if (symbolLengthBytes > MaxSymbolLength) {
            log_error("Too long symbol length: %d bytes; parser supports max %d bytes", symbolLengthBytes, MaxSymbolLength);
            return false;
        }

        if (!LRUCachedFileReader_readBytes(lruCachedFileReader, symbolBuffer, symbolLengthBytes)) {
            log_error("Error while reading symbol string");
            return false;
        }
        symbolBuffer[symbolLengthBytes] = 0;

        uint32_t symbolOffset;
        if (!LRUCachedFileReader_readU32BigEndian(lruCachedFileReader, &symbolOffset)) {
            log_error("Error while reading symbol offset");
            return false;
        }

        log_debug("Debug symbol: %s, offset: %d", symbolBuffer, symbolOffset);

        const char* testCaseName;
        if (testCaseName = HunkFileParser_getTestCaseNameFromSymbol(symbolBuffer)) {

            log_info("Testcase: %s, offset: %d", testCaseName, symbolOffset);
        }
    }

    log_error("parseSymbols is not yet implemented");
    return false;
}

bool HunkFileParser_findTests(LRUCachedFile* lruCachedFile) {

    HunkHeader hunkHeader;
    if (!HunkFileParser_readHunkHeader(lruCachedFile, &hunkHeader)) {
        return false;
    }

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(lruCachedFile, &lruCachedFileReader);
    LRUCachedFileReader_skipAhead(&lruCachedFileReader, hunkHeader.Size);

    int hunkId = -1;

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

			case HUNK_SYMBOL: if (!HunkFileParser_parseSymbols(&lruCachedFileReader, hunkId)) { return false; } break;

			case HUNK_DEBUG: if (!HunkFileParser_skipCodeDataDebug(&lruCachedFileReader)) { return false; } break;

			case HUNK_CODE: 
			case HUNK_DATA: hunkId++; if (!HunkFileParser_skipCodeDataDebug(&lruCachedFileReader)) { return false; } break;

			case HUNK_BSS: hunkId++; if (!HunkFileParser_skipBss(&lruCachedFileReader)) { return false; } break;

			case HUNK_RELOC32: if (!HunkFileParser_skipReloc32(&lruCachedFileReader)) { return false; } break;

			case HUNK_DREL32:
			case HUNK_RELOC32SHORT: if (!HunkFileParser_skipDreloc32(&lruCachedFileReader)) { return false; } break;

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
