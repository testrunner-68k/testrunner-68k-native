#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include "HunkFileParser.h"
#include "LinearAllocator.h"
#include "LRUCachedFile.h"
#include "TestDescriptor.h"
extern "C" {
#include "log.h"
}

using ::testing::ElementsAre;

namespace {
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

    const char* TestFileName = "../../bin/minimal_c.exe";
}

TEST(HunkFileParser, SucceedsWithValidExecutable) {

    LRUCachedFile lruCachedFile;
    ASSERT_TRUE(LRUCachedFile_open(&lruCachedFile, TestFileName));

    LinearAllocator linearAllocator;
    uint8_t buffer[65536];
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    int numTests;
    ::TestDescriptor* tests;
    ASSERT_TRUE(HunkFileParser_findTests(&lruCachedFile, &linearAllocator, &numTests, &tests));
    ASSERT_EQ(3, numTests);
    ASSERT_EQ(std::string("TestCase1"), std::string(tests[0].Name));
    ASSERT_EQ(0, tests[0].Hunk);
    ASSERT_EQ(std::string("TestCase2"), std::string(tests[1].Name));
    ASSERT_EQ(0, tests[0].Hunk);
    ASSERT_EQ(std::string("TestCase3"), std::string(tests[2].Name));
    ASSERT_EQ(0, tests[0].Hunk);
}
