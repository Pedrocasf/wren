#ifndef wren_math_h
#define wren_math_h

#include <math.h>
#include <stdint.h>

// A union to let us reinterpret a float as raw bits and back.
typedef union
{
  uint32_t bits32;
  uint16_t bits16[2];
  float num;
} WrenFloatBits;

#define WREN_FLOAT_QNAN_POS_MIN_BITS (UINT32_C(0x7F800000))
#define WREN_FLOAT_QNAN_POS_MAX_BITS (UINT32_C(0x7FFFFFFF))

#define WREN_FLOAT_NAN (wrenFloatFromBits(WREN_FLOAT_QNAN_POS_MIN_BITS))

static inline float wrenFloatFromBits(uint32_t bits)
{
  WrenFloatBits data;
  data.bits32 = bits;
  return data.num;
}

static inline uint32_t wrenFloatToBits(float num)
{
  WrenFloatBits data;
  data.num = num;
  return data.bits32;
}

#endif
