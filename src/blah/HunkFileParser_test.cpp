#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HunkFileParser.h"
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

const char* TestFileName = "../bin/minimal_c.exe";

TEST(HunkFileParser, SucceedsWithValidExecutable) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    ASSERT_TRUE(HunkFileParser_findTests(&lruCachedFile));
}
