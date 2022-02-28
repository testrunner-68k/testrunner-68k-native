#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "LRUCachedFile.h"
extern "C" {
#include "log.h"
}

using ::testing::ElementsAre;

// Hide debug log messages

class SetLogLevel : public ::testing::Environment {
 public:
  ~SetLogLevel() override {}

  void SetUp() override {
      log_set_level(LOG_INFO);
  }

  void TearDown() override {}
};

testing::Environment* const foo_env = testing::AddGlobalTestEnvironment(new SetLogLevel());


const char* TestFileName = "../../src/blah/testfile.dat";
const int TestFileSize = 5 * 4096 - 4;

TEST(LRUCachedFile, OpenFailsWithInvalidFileName) {

    LRUCachedFile lruCachedFile;
    ASSERT_FALSE(LRUCachedFile_open(&lruCachedFile, "invalid_filename.txt"));
}

TEST(LRUCachedFile, OpenSucceedsWithValidFileName) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    ASSERT_EQ(TestFileSize, lruCachedFile.FileSize);

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, ValidReadSucceeds) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    uint8_t buffer[10] = { 0 };

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 0, 5));

    ASSERT_THAT(buffer, ElementsAre('h', 'e', 'l', 'l', 'o', 0, 0, 0, 0, 0 ));

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, ValidReadWithOffsetSucceeds) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    uint8_t buffer[10] = { 0 };

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 1, 3));

    ASSERT_THAT(buffer, ElementsAre('e', 'l', 'l', 0, 0, 0, 0, 0, 0, 0 ));

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, PageHits) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    uint8_t buffer[1] = { 0 };

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 0 * 4096, 1));

    // The first cache entry caches page 0, and is the most-recently accessed cache entry
    ASSERT_EQ(0, lruCachedFile.CachedPages[0].PageIndex);
    ASSERT_EQ(lruCachedFile.SequenceId, lruCachedFile.CachedPages[0].SequenceId);
    ASSERT_EQ(-1, lruCachedFile.CachedPages[1].PageIndex);
    ASSERT_EQ(-1, lruCachedFile.CachedPages[2].PageIndex);

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 4 * 4096, 1));

    // The first cache entry caches page 0
    ASSERT_EQ(0, lruCachedFile.CachedPages[0].PageIndex);
    ASSERT_NE(lruCachedFile.SequenceId, lruCachedFile.CachedPages[0].SequenceId);
    // The second cache entry caches page 4, and is the most-recently accessed cache entry
    ASSERT_EQ(4, lruCachedFile.CachedPages[1].PageIndex);
    ASSERT_EQ(lruCachedFile.SequenceId, lruCachedFile.CachedPages[1].SequenceId);
    ASSERT_EQ(-1, lruCachedFile.CachedPages[2].PageIndex);

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 0 * 4096, 1));

    // The first cache entry caches page 0, and is the most-recently accessed cache entry
    ASSERT_EQ(0, lruCachedFile.CachedPages[0].PageIndex);
    ASSERT_EQ(lruCachedFile.SequenceId, lruCachedFile.CachedPages[0].SequenceId);
    // The second cache entry caches page 4
    ASSERT_EQ(4, lruCachedFile.CachedPages[1].PageIndex);
    ASSERT_NE(lruCachedFile.SequenceId, lruCachedFile.CachedPages[1].SequenceId);
    ASSERT_EQ(-1, lruCachedFile.CachedPages[2].PageIndex);

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, LargeReadSucceeds) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    uint8_t buffer[TestFileSize] = { 0 };

    ASSERT_TRUE(LRUCachedFile_readAt(&lruCachedFile, buffer, 0, TestFileSize));

    ASSERT_THAT(std::vector(buffer + 0 * 4096, buffer + 0 * 4096 + 5), ElementsAre('h', 'e', 'l', 'l', 'o'));
    ASSERT_THAT(std::vector(buffer + 1 * 4096, buffer + 1 * 4096 + 5), ElementsAre('w', 'o', 'r', 'l', 'd'));
    ASSERT_THAT(std::vector(buffer + 2 * 4096, buffer + 2 * 4096 + 5), ElementsAre('t', 'h', 'r', 'e', 'e'));
    ASSERT_THAT(std::vector(buffer + 3 * 4096, buffer + 3 * 4096 + 5), ElementsAre('f', 'o', 'u', 'r', '4'));
    ASSERT_THAT(std::vector(buffer + 4 * 4096, buffer + 4 * 4096 + 5), ElementsAre('f', 'i', 'v', 'e', '4'));

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, ReadPastEndOfFileFails) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    ASSERT_FALSE(LRUCachedFile_readAt(&lruCachedFile, 0, 5*4096-4-2, 4));

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFile, ReadPastStartOfFileFails) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    ASSERT_FALSE(LRUCachedFile_readAt(&lruCachedFile, 0, -2, 4));

    LRUCachedFile_close(&lruCachedFile);
}
