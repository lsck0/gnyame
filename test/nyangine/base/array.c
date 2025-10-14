#include "nyangine/base/array.h"

#include "nyangine/base/arena.h"
#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_new();

  // === Basic creation and add ===
  s32Array xs = nya_array_new(arena, s32);
  nya_range_for (i, 0, 24) nya_array_add(&xs, i);
  nya_assert(xs.length == 24);

  // === foreach ===
  s32 sum = 0;
  nya_array_foreach (&xs, x) sum += *x;
  nya_assert(sum == 276);

  // === insert and remove ===
  nya_array_insert(&xs, 100, 2);
  nya_assert(nya_array_get(&xs, 2) == 100);
  s32 removed = nya_array_remove(&xs, 2);
  nya_assert(removed == 100);
  nya_assert(nya_array_get(&xs, 2) == 2);

  // === set ===
  nya_array_set(&xs, 200, 3);
  nya_assert(nya_array_get(&xs, 3) == 200);

  // === push and pop ===
  nya_array_push(&xs, 999);
  nya_assert(nya_array_last(&xs) == 999);
  s32 popped = nya_array_pop(&xs);
  nya_assert(popped == 999);
  nya_assert(xs.length == 24);

  // === push_front / pop_front ===
  nya_array_push_front(&xs, -1);
  nya_assert(nya_array_first(&xs) == -1);
  s32 first_removed = nya_array_pop_front(&xs);
  nya_assert(first_removed == -1);

  // === reserve and resize ===
  u32 old_capacity = xs.capacity;
  nya_array_reserve(&xs, old_capacity * 4UL);
  nya_assert(xs.capacity >= old_capacity * 4UL);

  // === clone ===
  s32Array xs_clone = nya_array_clone(&xs);
  nya_assert(xs_clone.length == xs.length);
  nya_assert(nya_array_equals(&xs, &xs_clone));
  nya_array_free(&xs_clone);

  // === slice ===
  s32Array part1 = nya_array_slice_incld(&xs, 0, 3);
  nya_assert(part1.length == 4);
  s32Array part2 = nya_array_slice_excld(&xs, 0, 4);
  nya_assert(part2.length == 4);
  nya_assert(nya_array_equals(&part1, &part2));

  // === extend ===
  s32Array ys = nya_array_new(arena, s32);
  nya_range_for (i, 0, 5) nya_array_add(&ys, i);
  u32 prev_len = xs.length;
  nya_array_extend(&xs, &ys);
  nya_assert(xs.length == prev_len + ys.length);

  // === contains and find ===
  nya_assert(nya_array_contains(&xs, 3));
  nya_assert(!nya_array_contains(&xs, 99999));
  nya_assert(nya_array_find(&xs, 3) >= 0);
  nya_assert(nya_array_find(&xs, 99999) == -1);

  // === reverse ===
  s32Array orig = nya_array_clone(&ys);
  nya_array_reverse(&ys);
  nya_assert(nya_array_get(&ys, 0) == 4);
  nya_array_reverse(&ys);
  nya_assert(nya_array_equals(&ys, &orig));
  nya_array_free(&orig);

  // === swap ===
  nya_array_swap(&ys, 0, 4);
  nya_assert(nya_array_get(&ys, 0) == 4);
  nya_assert(nya_array_get(&ys, 4) == 0);

  // === clear ===
  nya_array_clear(&ys);
  nya_assert(ys.length == 0);

  // === move ===
  NYA_Arena* arena2 = nya_arena_new();
  nya_array_move(&xs, arena2);
  nya_assert(xs.arena == arena2);

  // === equality ===
  s32Array zs = nya_array_new(arena2, s32);
  nya_range_for (i, 0, 5) nya_array_add(&zs, i);
  nya_assert(!nya_array_equals(&xs, &zs));

  // === free ===
  nya_array_free(&xs);
  nya_array_free(&ys);
  nya_array_free(&zs);

  // === cleanup ===
  nya_arena_garbage_collect(arena);
  nya_arena_destroy(arena);
  nya_arena_garbage_collect(arena2);
  nya_arena_destroy(arena2);

  return 0;
}
