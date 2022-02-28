#include "Endian.h"

// Define ENDIAN_BIG_ENDIAN or ENDIAN_LITTLE_ENDIAN depending on host type
// Define ENDIAN_BYTE_ORDER as 1234 or 4321 depending on host type

#if defined (__GLIBC__)
#include <endian.h>
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
	#define ENDIAN_LITTLE_ENDIAN
#elif (__BYTE_ORDER == __BIG_ENDIAN)
	#define ENDIAN_BIG_ENDIAN
#elif (__BYTE_ORDER == __PDP_ENDIAN)
	#define ENDIAN_BIG_ENDIAN
#else
	#error Unknown machine endianness detected.
#endif
	#define ENDIAN_BYTE_ORDER __BYTE_ORDER
#elif defined(_BIG_ENDIAN)
	#define ENDIAN_BIG_ENDIAN
	#define ENDIAN_BYTE_ORDER 4321
#elif defined(_LITTLE_ENDIAN)
	#define ENDIAN_LITTLE_ENDIAN
	#define ENDIAN_BYTE_ORDER 1234
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__) \
   || defined(__m68k__)
	#define ENDIAN_BIG_ENDIAN
	#define ENDIAN_BYTE_ORDER 4321
#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64)
	#define ENDIAN_LITTLE_ENDIAN
	#define ENDIAN_BYTE_ORDER 1234
#else
	#error "Unable to detect endian for your target."
#endif

static inline uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}

static inline uint32_t swap_uint32(uint32_t val)
{
    return ((val & 0x000000ff) << 24) |
           ((val & 0x0000ff00) << 8) |
           ((val & 0x00ff0000) >> 8) |
           ((val & 0xff000000) >> 24);
}

uint16_t Endian_readU16BigEndian(const uint16_t* value)
{
#if defined ENDIAN_BIG_ENDIAN
    return *value;
#else
    return swap_uint16(*value);
#endif
}

uint32_t Endian_readU32BigEndian(const uint32_t* value)
{
#if defined ENDIAN_BIG_ENDIAN
    return *value;
#else
    return swap_uint32(*value);
#endif
}
