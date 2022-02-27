#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint16_t Endian_readU16BigEndian(const uint16_t* value);

uint32_t Endian_readU32BigEndian(const uint32_t* value);

#ifdef __cplusplus
}
#endif
