#pragma once

#include <SDL3/SDL_stdinc.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

#define U8_MIN  0U
#define U8_MAX  255U
#define U16_MIN 0U
#define U16_MAX 65535U
#define U32_MIN 0UL
#define U32_MAX 4294967295UL
#define U64_MIN 0ULL
#define U64_MAX 18446744073709551615ULL

#define S8_MIN  (-128)
#define S8_MAX  127
#define S16_MIN (-32768)
#define S16_MAX 32767
#define S32_MIN (-2147483648L)
#define S32_MAX 2147483647L
#define S64_MIN (-9223372036854775808LL)
#define S64_MAX 9223372036854775807LL

// use like: nya_debug("number: "FMTu64"\n", number)
#define FMTu8  "%" PRIu8
#define FMTu16 "%" PRIu16
#define FMTu32 "%" PRIu32
#define FMTu64 "%" PRIu64
#define FMTs8  "%" PRId8
#define FMTs16 "%" PRId16
#define FMTs32 "%" PRId32
#define FMTs64 "%" PRId64
#define FMTf32 "%f"
#define FMTf64 "%lf"

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(s8) == 1);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(s64) == 8);
static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);
