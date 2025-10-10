#pragma once

#define nya_unused(...) ((void)(0, __VA_ARGS__))

#define nya_likely(expr)   __builtin_expect(!!(expr), 1)
#define nya_unlikely(expr) __builtin_expect(!!(expr), 0)

#define nya_assert_type_match(a, b) static_assert(__builtin_types_compatible_p(typeof(a), typeof(b)), "Incompatible types.")

#define nya_sizeof_field(type, member) sizeof((((type*)0)->member))
#define nya_typeof_field(type, member) typeof(((type*)0)->member)
#define nya_offsetof(type, member)     __builtin_offsetof(type, member)
#define nya_offsetof_end(type, member) (offsetof(type, member) + sizeof_field(type, member))
#define nya_container_of(ptr, type, member)                                                                                                          \
  _Generic(                                                                                                                                          \
      ptr,                                                                                                                                           \
      const typeof(*(ptr))*: ((const type*)_nya_raw_container_of(ptr, type, member)),                                                                \
      default: ((type*)_nya_raw_container_of(ptr, type, member))                                                                                     \
  )
#define _nya_raw_container_of(ptr, type, member)                                                                                                     \
  ({                                                                                                                                                 \
    void* ptr_var = (void*)(ptr);                                                                                                                    \
    assert_type_match(*(ptr), ((type*)0)->member);                                                                                                   \
    ((type*)(ptr_var - offsetof(type, member)));                                                                                                     \
  })

#define nya_flag_set(flags, flag)    ((flags) |= (flag))
#define nya_flag_unset(flags, flag)  ((flags) &= ~(flag))
#define nya_flag_toggle(flags, flag) ((flags) ^= (flag))
#define nya_flag_check(flags, flag)  ((flags) & (flag))
#define nya_flag_equals(flags, flag) ((flags) == (flag))

#define nya_bit_set(val, bit)             ((val) |= (1 << (bit)))
#define nya_bit_unset(val, bit)           ((val) &= ~(1 << (bit)))
#define nya_bit_check(val, bit)           ((val) & (1 << (bit)))
#define nya_bit_toggle(val, bit)          ((val) ^= (1 << (bit)))
#define nya_bit_count_leading_zeros(val)  __builtin_clz(val)
#define nya_bit_count_trailing_zeros(val) __builtin_ctz(val)

#define nya_byte_to_kibyte(val) ((val) << 10)
#define nya_byte_to_mebyte(val) ((val) << 20)
#define nya_byte_to_gibyte(val) ((val) << 30)
#define nya_byte_to_tebyte(val) ((val) << 40)
#define nya_byte_to_kbyte(val)  ((val) * 1000)
#define nya_byte_to_mbyte(val)  ((val) * 1000000)
#define nya_byte_to_gbyte(val)  ((val) * 1000000000)
#define nya_byte_to_tbyte(val)  ((val) * 1000000000000LL)
