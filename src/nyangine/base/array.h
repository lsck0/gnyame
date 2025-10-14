#pragma once

#include <stdio.h>

#include "nyangine/base/arena.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/logging.h"
#include "nyangine/base/math.h"
#include "nyangine/base/memory.h"
#include "nyangine/base/misc.h"

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

#define nya_range_for_t(type, idx, start, end) for (type idx = start; (idx) < (end); (idx)++)
#define nya_range_for(idx, start, end)         nya_range_for_t (s32, idx, start, end)

#define nya_array_new(arena_ptr, item_type)                                                                            \
  {                                                                                                                    \
      .items    = nya_arena_alloc(arena_ptr, _NYA_ARRAY_DEFAULT_SIZE * sizeof(item_type)),                             \
      .length   = 0,                                                                                                   \
      .capacity = _NYA_ARRAY_DEFAULT_SIZE,                                                                             \
      .arena    = (arena_ptr),                                                                                         \
  }

#define nya_array_new_with_capacity(arena_ptr, item_type, capacity)                                                    \
  {                                                                                                                    \
      .items    = nya_arena_alloc(arena_ptr, (capacity) * sizeof(item_type)),                                          \
      .length   = 0,                                                                                                   \
      .capacity = (capacity),                                                                                          \
      .arena    = (arena_ptr),                                                                                         \
  }

#define nya_array_from_carray(arena_ptr, item_type, carray, carray_length)                                             \
  ({                                                                                                                   \
    nya_assert_type_match(arena_ptr, (NYA_Arena*)0);                                                                   \
    nya_assert_type_match(carray, (item_type*)0);                                                                      \
    nya_assert_type_match(carray_length, (u64)0);                                                                      \
    item_type##Array arr = nya_array_new_with_capacity(arena_ptr, item_type, carray_length);                           \
    nya_range_for (idx, 0, carray_length) { nya_array_push(&arr, (carray)[idx]); }                                     \
    arr;                                                                                                               \
  })

#define nya_array_from_argv(arena_ptr, argc, argv)                                                                     \
  ({                                                                                                                   \
    nya_assert_type_match(arena_ptr, (NYA_Arena*)0);                                                                   \
    nya_assert_type_match(argv, (const char**)0);                                                                      \
    NYA_StringArray args = nya_array_new_with_capacity(&arena, string, argc);                                          \
    nya_range_for (idx, 0, argc) { nya_array_push(&args, nya_string_from(&arena, (argv)[idx])); }                      \
    args;                                                                                                              \
  })

#define nya_array_resize(arr_ptr, arr_capacity)                                                                        \
  do {                                                                                                                 \
    (arr_ptr)->items = nya_arena_realloc(                                                                              \
        (arr_ptr)->arena,                                                                                              \
        (arr_ptr)->items,                                                                                              \
        (arr_ptr)->capacity * sizeof(*(arr_ptr)->items),                                                               \
        (arr_capacity) * sizeof(*(arr_ptr)->items)                                                                     \
    );                                                                                                                 \
    (arr_ptr)->capacity = arr_capacity;                                                                                \
  } while (0)

#define nya_array_reserve(arr_ptr, arr_capacity)                                                                       \
  do {                                                                                                                 \
    if ((arr_ptr)->capacity < (arr_capacity)) nya_array_resize(arr_ptr, arr_capacity);                                 \
  } while (0)

#define nya_array_add(arr_ptr, item)                                                                                   \
  do {                                                                                                                 \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    if ((arr_ptr)->length == (arr_ptr)->capacity) nya_array_resize(arr_ptr, 2UL * (arr_ptr)->capacity);                \
    (arr_ptr)->items[(arr_ptr)->length++] = (item);                                                                    \
  } while (0)

#define nya_array_extend(arr_ptr, other_arr_ptr)                                                                       \
  do {                                                                                                                 \
    nya_assert_type_match((arr_ptr)->items[0], (other_arr_ptr)->items[0]);                                             \
    nya_array_reserve(arr_ptr, (arr_ptr)->length + (other_arr_ptr)->length);                                           \
    for (u64 i = 0; i < (other_arr_ptr)->length; i++) nya_array_add(arr_ptr, (other_arr_ptr)->items[i]);               \
  } while (0)

#define nya_array_insert(arr_ptr, item, index)                                                                         \
  do {                                                                                                                 \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    if ((arr_ptr)->length == (arr_ptr)->capacity) nya_array_resize(arr_ptr, 2UL * (arr_ptr)->capacity);                \
    nya_memmove(                                                                                                       \
        (arr_ptr)->items + (index) + 1,                                                                                \
        (arr_ptr)->items + (index),                                                                                    \
        ((arr_ptr)->length * sizeof(*(arr_ptr)->items) - (index))                                                      \
    );                                                                                                                 \
    (arr_ptr)->items[index] = (item);                                                                                  \
    (arr_ptr)->length++;                                                                                               \
  } while (0)

#define nya_array_remove(arr_ptr, index)                                                                               \
  ({                                                                                                                   \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    typeof(*(arr_ptr)->items) item = (arr_ptr)->items[index];                                                          \
    if ((index) != (arr_ptr)->length - 1) {                                                                            \
      nya_memmove(                                                                                                     \
          (arr_ptr)->items + (index),                                                                                  \
          (arr_ptr)->items + (index) + 1,                                                                              \
          ((arr_ptr)->length * sizeof(*(arr_ptr)->items) - (index) - 1)                                                \
      );                                                                                                               \
    }                                                                                                                  \
    (arr_ptr)->length--;                                                                                               \
    item;                                                                                                              \
  })

#define nya_array_push_back(arr_ptr, item)  nya_array_add(arr_ptr, item)
#define nya_array_push_front(arr_ptr, item) nya_array_insert(arr_ptr, item, 0)
#define nya_array_pop_back(arr_ptr)         nya_array_remove(arr_ptr, (arr_ptr)->length - 1)
#define nya_array_pop_front(arr_ptr)        nya_array_remove(arr_ptr, 0)
#define nya_array_push(arr_ptr, item)       nya_array_add(arr_ptr, item)
#define nya_array_pop(arr_ptr)              nya_array_remove(arr_ptr, (arr_ptr)->length - 1)
#define nya_array_enqueue(arr_ptr, item)    nya_array_add(arr_ptr, item)
#define nya_array_dequeue(arr_ptr)          nya_array_remove(arr_ptr, 0)

#define nya_array_get(arr_ptr, index)                                                                                  \
  ({                                                                                                                   \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    (arr_ptr)->items[index];                                                                                           \
  })

#define nya_array_first(arr_ptr) nya_array_get(arr_ptr, 0)
#define nya_array_last(arr_ptr)  nya_array_get(arr_ptr, (arr_ptr)->length - 1)

#define nya_array_set(arr_ptr, item, index)                                                                            \
  do {                                                                                                                 \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    (arr_ptr)->items[index] = item;                                                                                    \
  } while (0)

#define nya_array_contains(arr_ptr, item)                                                                              \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    typeof((arr_ptr)->items[0]) item_var = item;                                                                       \
    bool                        contains = false;                                                                      \
    nya_array_for (arr_ptr, arr_index) {                                                                               \
      if (nya_memcmp(&(arr_ptr)->items[arr_index], &item_var, sizeof(item_var)) == 0) {                                \
        contains = true;                                                                                               \
        break;                                                                                                         \
      }                                                                                                                \
    }                                                                                                                  \
    contains;                                                                                                          \
  })

#define nya_array_find(arr_ptr, item)                                                                                  \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    typeof((arr_ptr)->items[0]) item_var = item;                                                                       \
    s64                         index    = 0;                                                                          \
    nya_array_for (arr_ptr, item_index) {                                                                              \
      if (nya_memcmp(&(arr_ptr)->items[item_index], &item_var, sizeof(item_var)) == 0) { break; }                      \
      index++;                                                                                                         \
    }                                                                                                                  \
    (u64) index == (arr_ptr)->length ? -1 : index;                                                                     \
  })

#define nya_array_clear(arr_ptr)                                                                                       \
  do { (arr_ptr)->length = 0; } while (0)

#define nya_array_free(arr_ptr)                                                                                        \
  do {                                                                                                                 \
    nya_arena_free((arr_ptr)->arena, (arr_ptr)->items, sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity);               \
    (arr_ptr)->items    = nullptr;                                                                                     \
    (arr_ptr)->length   = 0;                                                                                           \
    (arr_ptr)->capacity = 0;                                                                                           \
  } while (0)

#define nya_array_clone(arr_ptr)                                                                                         \
  ({                                                                                                                     \
    typeof(*(arr_ptr)) copy = {                                                                                          \
        .items    = nya_arena_copy((arr_ptr)->arena, (arr_ptr)->items, sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity), \
        .length   = (arr_ptr)->length,                                                                                   \
        .capacity = (arr_ptr)->capacity,                                                                                 \
        .arena    = (arr_ptr)->arena                                                                                     \
    };                                                                                                                   \
    copy;                                                                                                                \
  })

#define nya_array_move(arr_ptr, new_arena_ptr)                                                                         \
  do {                                                                                                                 \
    nya_assert_type_match(new_arena_ptr, (arr_ptr)->arena);                                                            \
    *(arr_ptr) = (typeof(*(arr_ptr))){.items = nya_arena_move(                                                         \
                                          (arr_ptr)->arena,                                                            \
                                          new_arena_ptr,                                                               \
                                          (arr_ptr)->items,                                                            \
                                          sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity                              \
                                      ),                                                                               \
                                      .length   = (arr_ptr)->length,                                                   \
                                      .capacity = (arr_ptr)->capacity,                                                 \
                                      .arena    = (new_arena_ptr)};                                                       \
  } while (0)

#define nya_array_slice_excld(arr_ptr, start, end)                                                                     \
  ({                                                                                                                   \
    _nya_array_access_guard(start, (arr_ptr)->length);                                                                 \
    _nya_array_access_guard((end) - 1, (arr_ptr)->length);                                                             \
    typeof(*(arr_ptr)) slice = {                                                                                       \
        .items    = &(arr_ptr)->items[start],                                                                          \
        .length   = (end) - (start),                                                                                   \
        .capacity = (end) - (start),                                                                                   \
        .arena    = nullptr,                                                                                           \
    };                                                                                                                 \
    slice;                                                                                                             \
  })

#define nya_array_slice_incld(arr_ptr, start, end)                                                                     \
  ({                                                                                                                   \
    _nya_array_access_guard(start, (arr_ptr)->length);                                                                 \
    _nya_array_access_guard(end, (arr_ptr)->length);                                                                   \
    typeof(*(arr_ptr)) slice = {                                                                                       \
        .items    = &(arr_ptr)->items[start],                                                                          \
        .length   = (end) - (start) + 1,                                                                               \
        .capacity = (end) - (start) + 1,                                                                               \
        .arena    = nullptr                                                                                            \
    };                                                                                                                 \
    slice;                                                                                                             \
  })

#define nya_array_swap(arr_ptr, index1, index2)                                                                        \
  do {                                                                                                                 \
    _nya_array_access_guard(index1, (arr_ptr)->length);                                                                \
    _nya_array_access_guard(index2, (arr_ptr)->length);                                                                \
    typeof(*(arr_ptr)->items) tmp = (arr_ptr)->items[index1];                                                          \
    (arr_ptr)->items[index1]      = (arr_ptr)->items[index2];                                                          \
    (arr_ptr)->items[index2]      = tmp;                                                                               \
  } while (0)

#define nya_array_reverse(arr_ptr)                                                                                     \
  do {                                                                                                                 \
    for (u32 i = 0; i < (arr_ptr)->length / 2; i++) nya_array_swap(arr_ptr, i, (arr_ptr)->length - i - 1);             \
  } while (0)

#define nya_array_equals(arr1_ptr, arr2_ptr)                                                                           \
  ({                                                                                                                   \
    nya_assert_type_match((arr1_ptr)->items[0], (arr2_ptr)->items[0]);                                                 \
    bool equal = (arr1_ptr)->length == (arr2_ptr)->length;                                                             \
    if (equal) {                                                                                                       \
      for (u32 i = 0; i < (arr1_ptr)->length; i++) {                                                                   \
        if (nya_memcmp(&(arr1_ptr)->items[i], &(arr2_ptr)->items[i], sizeof((arr1_ptr)->items[i])) != 0) {             \
          equal = false;                                                                                               \
          break;                                                                                                       \
        }                                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
    equal;                                                                                                             \
  })

#define nya_array_for(arr_ptr, index_name) for (u64 index_name = 0; (index_name) < (arr_ptr)->length; (index_name)++)

#define nya_array_for_reverse(arr_ptr, index_name)                                                                     \
  for (u32 index_name = (arr_ptr)->length - 1; (index_name) >= 0; (index_name)--)

#define nya_array_foreach(arr_ptr, item_name)                                                                          \
  for (typeof(*(arr_ptr)->items)* item_name##_ptr = (arr_ptr)->items;                                                  \
       item_name##_ptr < (arr_ptr)->items + (arr_ptr)->length;                                                         \
       item_name##_ptr++)                                                                                              \
    for (typeof (*(arr_ptr)->items)(item_name) = *item_name##_ptr, *_break = &(item_name); _break; _break = 0)

#define nya_array_foreach_reverse(arr_ptr, item_name)                                                                  \
  for (typeof(*(arr_ptr)->items)* item_name##_ptr = (arr_ptr)->items + (arr_ptr)->length - 1;                          \
       item_name##_ptr >= (arr_ptr)->items;                                                                            \
       item_name##_ptr--)                                                                                              \
    for (typeof (*(arr_ptr)->items)(item_name) = *item_name##_ptr, *_break = &(item_name); _break; _break = 0)

#define nya_array_print(arr_ptr, item_fmt)                                                                             \
  do {                                                                                                                 \
    printf("[");                                                                                                       \
    nya_array_foreach (arr_ptr, item) {                                                                                \
      printf(item_fmt, item);                                                                                          \
      if (item != (arr_ptr)->items[(arr_ptr)->length - 1]) printf(", ");                                               \
    }                                                                                                                  \
    printf("]\n");                                                                                                     \
  } while (0)
