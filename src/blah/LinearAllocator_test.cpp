#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "LinearAllocator.h"
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

TEST(LinearAllocator, AllocationMovesAllocPtrForward) {

    uint8_t buffer[16];
    LinearAllocator linearAllocator;
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    uint8_t* bufptr;
    ASSERT_TRUE(LinearAllocator_allocate(&linearAllocator, 6, &bufptr));
    ASSERT_EQ(buffer + 0, bufptr);
    ASSERT_TRUE(LinearAllocator_allocate(&linearAllocator, 2, &bufptr));
    ASSERT_EQ(buffer + 8, bufptr);
    ASSERT_TRUE(LinearAllocator_allocate(&linearAllocator, 2, &bufptr));
    ASSERT_EQ(buffer + 12, bufptr);
}

TEST(LinearAllocator, AllocationFailsIfOutOfSpace) {

    uint8_t buffer[16];
    LinearAllocator linearAllocator;
    LinearAllocator_init(&linearAllocator, buffer, sizeof(buffer));

    uint8_t* bufptr;
    ASSERT_TRUE(LinearAllocator_allocate(&linearAllocator, 10, &bufptr));
    ASSERT_EQ(buffer + 0, bufptr);
    // Not enough room - allocation will fail, and will not consume allocator memory
    ASSERT_FALSE(LinearAllocator_allocate(&linearAllocator, 10, &bufptr));
    ASSERT_TRUE(LinearAllocator_allocate(&linearAllocator, 2, &bufptr));
    ASSERT_EQ(buffer + 12, bufptr);
}
