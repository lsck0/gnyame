#pragma once

#define nya_unused(...) ((void)(0, __VA_ARGS__))

#define nya_likely(expr)   __builtin_expect(!!(expr), 1)
#define nya_unlikely(expr) __builtin_expect(!!(expr), 0)

#define nya_byte_to_kibyte(val) ((val) << 10)
#define nya_byte_to_mebyte(val) ((val) << 20)
#define nya_byte_to_gibyte(val) ((val) << 30)
#define nya_byte_to_tebyte(val) ((val) << 40)
#define nya_byte_to_kbyte(val)  ((val) * 1000)
#define nya_byte_to_mbyte(val)  ((val) * 1000000)
#define nya_byte_to_gbyte(val)  ((val) * 1000000000)
#define nya_byte_to_tbyte(val)  ((val) * 1000000000000LL)
