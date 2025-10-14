#include "nyangine/base/arena.h"

#include <stdio.h>

#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"

s32 main(void) {
  // == create ==
  NYA_Arena* arena = nya_arena_new();
  nya_assert(arena != nullptr);

  // == basic alloc & free ==
  void* thing = nya_arena_alloc(arena, 100);
  nya_assert(thing != nullptr);
  nya_arena_free(arena, thing, 100);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == realloc grow & shrink ==
  thing = nya_arena_alloc(arena, 100);
  thing = nya_arena_realloc(arena, thing, 100, 1000);
  thing = nya_arena_realloc(arena, thing, 1000, 50);
  nya_arena_free(arena, thing, 50);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == free all ==
  for (s32 i = 0; i < 10; i++) nya_arena_alloc(arena, 128 * (i + 1));
  nya_assert(nya_arena_memory_usage(arena) > 0);
  nya_arena_free_all(arena);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == garbage collect ==
  void* a = nya_arena_alloc(arena, 200);
  void* b = nya_arena_alloc(arena, 400);
  nya_arena_free(arena, a, 200);
  nya_arena_garbage_collect(arena);
  nya_arena_free(arena, b, 400);
  nya_arena_garbage_collect(arena);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == new with options (alignment / region size) ==
  NYA_Arena* arena2      = nya_arena_new_with_options(16, 4096, true);
  void*      aligned_ptr = nya_arena_alloc(arena2, 512);
  nya_assert(((uintptr_t)aligned_ptr % 16) == 0);
  nya_arena_free(arena2, aligned_ptr, 512);
  nya_assert(nya_arena_memory_usage(arena2) == 0);

  // == copy & move ==
  u8* src_buf = (u8*)nya_arena_alloc(arena, 256);
  for (u32 i = 0; i < 256; i++) src_buf[i] = (u8)i;
  u8* copy_buf = (u8*)nya_arena_copy(arena2, src_buf, 256);
  for (u32 i = 0; i < 256; i++) nya_assert(copy_buf[i] == src_buf[i]);
  u8* moved_buf = (u8*)nya_arena_move(arena, arena2, src_buf, 256);
  nya_assert(moved_buf != nullptr);
  nya_arena_free(arena2, copy_buf, 256);
  nya_arena_free(arena2, moved_buf, 256);
  nya_arena_garbage_collect(arena);
  nya_arena_garbage_collect(arena2);

  // == mass alloc & fragmentation ==
  void* ptrs[100];
  for (s32 i = 0; i < 100; i++) ptrs[i] = nya_arena_alloc(arena, (i + 1) * 64);
  for (s32 i = 0; i < 50; i++) nya_arena_free(arena, ptrs[i], (i + 1) * 64);
  for (s32 i = 50; i < 100; i += 2) nya_arena_free(arena, ptrs[i], (i + 1) * 64);
  nya_arena_garbage_collect(arena);
  nya_assert(nya_arena_memory_usage(arena) >= 0);
  nya_arena_free_all(arena);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == alignment check ==
  void* p1 = nya_arena_alloc(arena, 1);
  void* p2 = nya_arena_alloc(arena, 7);
  void* p3 = nya_arena_alloc(arena, 8);
  nya_assert(((uintptr_t)p1 % 8) == 0);
  nya_assert(((uintptr_t)p2 % 8) == 0);
  nya_assert(((uintptr_t)p3 % 8) == 0);
  nya_arena_free_all(arena);
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == interleaved realloc ==
  void* block = nya_arena_alloc(arena, 100);
  block       = nya_arena_realloc(arena, block, 100, 1000);
  nya_assert(block != nullptr);
  block = nya_arena_realloc(arena, block, 1000, 10000);
  nya_assert(block != nullptr);
  nya_arena_free(arena, block, 10000);

  // == stress test (alloc/free loop) ==
  for (s32 i = 1; i <= 24; i++) {
    void* t = nya_arena_alloc(arena, 100 * i);
    nya_arena_free(arena, t, 100 * i);
  }
  nya_assert(nya_arena_memory_usage(arena) == 0);

  // == final cleanup ==
  nya_arena_garbage_collect(arena);
  nya_arena_garbage_collect(arena2);
  nya_assert(nya_arena_memory_usage(arena) == 0);
  nya_assert(nya_arena_memory_usage(arena2) == 0);
  nya_arena_destroy(arena);
  nya_arena_destroy(arena2);

  return 0;
}
