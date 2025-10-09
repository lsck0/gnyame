#pragma once

#define unused(...) ((void)(0, __VA_ARGS__))

#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)

#define byte_to_kibyte(val) ((val) << 10)
#define byte_to_mebyte(val) ((val) << 20)
#define byte_to_gibyte(val) ((val) << 30)
#define byte_to_tebyte(val) ((val) << 40)
#define byte_to_kbyte(val)  ((val) * 1000)
#define byte_to_mbyte(val)  ((val) * 1000000)
#define byte_to_gbyte(val)  ((val) * 1000000000)
#define byte_to_tbyte(val)  ((val) * 1000000000000LL)
