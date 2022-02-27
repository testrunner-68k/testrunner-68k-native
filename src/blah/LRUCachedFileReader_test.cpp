#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "LRUCachedFileReader.h"
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

const char* TestFileName = "../src/blah/testfile.dat";

TEST(LRUCachedFileReader, ReadsU16BigEndianValuesAndAdvances) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(&lruCachedFile, &lruCachedFileReader);

    uint16_t value0;
    ASSERT_TRUE(LRUCachedFileReader_readU16BigEndian(&lruCachedFileReader, &value0));
    ASSERT_EQ(('h' << 8) | ('e' << 0), value0);

    uint16_t value1;
    ASSERT_TRUE(LRUCachedFileReader_readU16BigEndian(&lruCachedFileReader, &value1));
    ASSERT_EQ(('l' << 8) | ('l' << 0), value1);

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFileReader, ReadsU32BigEndianValuesAndAdvances) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(&lruCachedFile, &lruCachedFileReader);

    uint32_t value0;
    ASSERT_TRUE(LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &value0));
    ASSERT_EQ(('h' << 24) | ('e' << 16) | ('l' << 8) | ('l' << 0), value0);

    uint32_t value1;
    ASSERT_TRUE(LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &value1));
    ASSERT_EQ(('o' << 24) | ('5' << 16) | ('6' << 8) | ('7' << 0), value1);

    LRUCachedFile_close(&lruCachedFile);
}

TEST(LRUCachedFileReader, SetPositionAffectsReads) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    LRUCachedFileReader lruCachedFileReader;
    LRUCachedFileReader_init(&lruCachedFile, &lruCachedFileReader);

    LRUCachedFileReader_setPosition(&lruCachedFileReader, 4);

    uint32_t value;
    ASSERT_TRUE(LRUCachedFileReader_readU32BigEndian(&lruCachedFileReader, &value));
    ASSERT_EQ(('o' << 24) | ('5' << 16) | ('6' << 8) | ('7' << 0), value);

    LRUCachedFile_close(&lruCachedFile);
}
