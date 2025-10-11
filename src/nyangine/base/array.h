#pragma once

#include "nyangine/base/arena.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/logging.h"
#include "nyangine/base/math.h"

#define _NYA_ARRAY_DEFAULT_SIZE 16

#define _nya_array_access_guard(index, length)                                                                         \
  if (nya_unlikely(0 > (index) || (index) >= (length))) {                                                              \
    nya_panic("Array index " FMTu32 " (length " FMTu32 ") out of bounds.", index, length);                             \
  }

#define nya_derive_array(type)                                                                                         \
  typedef struct {                                                                                                     \
    u32        length;                                                                                                 \
    u32        capacity;                                                                                               \
    type*      items;                                                                                                  \
    NYA_Arena* arena;                                                                                                  \
  } type##Array

nya_derive_array(u8);
nya_derive_array(u16);
nya_derive_array(u32);
nya_derive_array(u64);
nya_derive_array(s8);
nya_derive_array(s16);
nya_derive_array(s32);
nya_derive_array(s64);
nya_derive_array(f32);
nya_derive_array(f64);

#if defined(__has_feature) && __has_attribute(ext_vector_type)
nya_derive_array(f32x2);
nya_derive_array(f32x3);
nya_derive_array(f32x4);
nya_derive_array(f64x2);
nya_derive_array(f64x3);
nya_derive_array(f64x4);
#endif // defined(__has_feature) && __has_attribute(ext_vector_type)

#if defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)
nya_derive_array(f32_2x2);
nya_derive_array(f32_3x3);
nya_derive_array(f32_4x4);
nya_derive_array(f64_2x2);
nya_derive_array(f64_3x3);
nya_derive_array(f64_4x4);
#endif // defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)

#define nya_array_new(arena_ptr, item_type)                                                                            \
  {                                                                                                                    \
      .items    = nya_arena_alloc(arena_ptr, ARRAY_DEFAULT_SIZE * sizeof(item_type)),                                  \
      .length   = 0,                                                                                                   \
      .capacity = ARRAY_DEFAULT_SIZE,                                                                                  \
      .arena    = (arena_ptr),                                                                                         \
  }

#define nya_array_new_with_capacity(arena_ptr, item_type, capacity)                                                    \
  {                                                                                                                    \
      .items    = nya_arena_alloc(arena_ptr, (capacity) * sizeof(item_type)),                                          \
      .length   = 0,                                                                                                   \
      .capacity = (capacity),                                                                                          \
      .arena    = (arena_ptr),                                                                                         \
  }
