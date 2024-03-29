#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Endian.h"

TEST(Endian, ReadU16BigEndianValueSucceeds) {

    union
    {
        uint8_t u8[2];
        uint16_t u16;
    } u8_and_u16 = { .u8 = { 0x12, 0x34 } };

    ASSERT_EQ(0x1234, Endian_readU16BigEndian(&u8_and_u16.u16));
}

TEST(Endian, ReadU32BigEndianValueSucceeds) {

    union
    {
        uint8_t u8[4];
        uint32_t u32;
    } u8_and_u32 = { .u8 = { 0x12, 0x34, 0x56, 0x78 } };

    ASSERT_EQ(0x12345678, Endian_readU32BigEndian(&u8_and_u32.u32));
}
