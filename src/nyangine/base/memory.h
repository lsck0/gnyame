#pragma once

#include "SDL3/SDL_stdinc.h"

#define nya_alloca  __builtin_alloca
#define nya_malloc  SDL_malloc
#define nya_realloc SDL_realloc
#define nya_calloc  SDL_calloc
#define nya_free    SDL_free

#define nya_memcmp  SDL_memcmp
#define nya_memcpy  SDL_memcpy
#define nya_memmove SDL_memmove
#define nya_memset  SDL_memset

#define nya_byte_to_kibyte(val) ((val) >> 10)
#define nya_byte_to_mebyte(val) ((val) >> 20)
#define nya_byte_to_gibyte(val) ((val) >> 30)
#define nya_byte_to_tebyte(val) ((val) >> 40)
#define nya_byte_to_kbyte(val)  ((val) / 1000)
#define nya_byte_to_mbyte(val)  ((val) / 1000000)
#define nya_byte_to_gbyte(val)  ((val) / 1000000000)
#define nya_byte_to_tbyte(val)  ((val) / 1000000000000LL)
#define nya_kibyte_to_byte(val) ((val) << 10)
#define nya_mebyte_to_byte(val) ((val) << 20)
#define nya_gibyte_to_byte(val) ((val) << 30)
#define nya_tebyte_to_byte(val) ((val) << 40)
#define nya_kbyte_to_byte(val)  ((val) * 1000)
#define nya_mbyte_to_byte(val)  ((val) * 1000000)
#define nya_gbyte_to_byte(val)  ((val) * 1000000000)
#define nya_tbyte_to_byte(val)  ((val) * 1000000000000LL)
