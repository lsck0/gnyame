#include "nyangine/base/arena.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/attributes.h"
#include "nyangine/base/misc.h"

#if defined(__has_feature) && __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
#define ASAN_ENABLED                            true
#define ASAN_PADDING                            64 // bytes
#define asan_poison_memory_region(addr, size)   __asan_poison_memory_region(addr, size)
#define asan_unpoison_memory_region(addr, size) __asan_unpoison_memory_region(addr, size)
#else
#define ASAN_ENABLED                            false
#define ASAN_PADDING                            0 // bytes
#define asan_poison_memory_region(addr, size)   ((void)(addr), (void)(size))
#define asan_unpoison_memory_region(addr, size) ((void)(addr), (void)(size))
#endif // defined(__has_feature) && __has_feature(address_sanitizer)

/*
 * ─────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────
 * */

typedef struct NYA_ArenaRegion       NYA_ArenaRegion;
typedef struct NYA_ArenaFreeList     NYA_ArenaFreeList;
typedef struct NYA_ArenaFreeListNode NYA_ArenaFreeListNode;

struct NYA_Arena {
  u32 default_size_region_size_bytes;
  u8  alignment;

  NYA_ArenaRegion* head;
};

struct NYA_ArenaRegion {
  u32 used;
  u32 capacity;
  u8* memory;

  NYA_ArenaFreeList* free_list;
  NYA_ArenaRegion*   next;
};

struct NYA_ArenaFreeList {
  u32 free_bytes;
  u32 largest_free_slot_bytes;

  NYA_ArenaFreeListNode* head;
};

struct AYA_ArenaFreeListNode {
  u32 offset;
  u32 size;

  NYA_ArenaFreeListNode* next;
};

__attr_maybe_unused static void nya_arena_align_and_pad_size(NYA_Arena* arena, u32* size);

/*
 * ─────────────────────────────────────────
 * NON-DEBUG PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_Arena* _nya_arena_nodebug_new(void) {
  nya_todo();
}

void* _nya_arena_nodebug_alloc(NYA_Arena* arena, u64 size) {
  unused(arena, size);

  nya_todo();
}

void* _nya_arena_nodebug_realloc(NYA_Arena* arena, const void* ptr, u64 old_size, u64 new_size) {
  unused(arena, ptr, old_size, new_size);

  nya_todo();
}

void _nya_arena_nodebug_free(NYA_Arena* arena, const void* ptr, u64 size) {
  unused(arena, ptr, size);

  nya_todo();
}

void _nya_arena_nodebug_free_all(NYA_Arena* arena) {
  unused(arena);

  nya_todo();
}

void _nya_arena_nodebug_destroy(NYA_Arena* arena) {
  unused(arena);

  nya_todo();
}

void* _nya_arena_nodebug_copy(NYA_Arena* dst, void* ptr, u64 size) {
  unused(dst, ptr, size);

  nya_todo();
}

void* _nya_arena_nodebug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size) {
  unused(src, dst, ptr, size);

  nya_todo();
}

/*
 * ─────────────────────────────────────────
 * DEBUG PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_Arena* _nya_arena_debug_new(const char* function, const char* file, u32 line) {
  unused(function, file, line);

  nya_todo();
}

void* _nya_arena_debug_alloc(NYA_Arena* arena, u64 size, const char* function, const char* file, u32 line) {
  unused(arena, size, function, file, line);

  nya_todo();
}

void* _nya_arena_debug_realloc(NYA_Arena* arena, const void* ptr, u64 old_size, u64 new_size, const char* function, const char* file, u32 line) {
  unused(arena, ptr, old_size, new_size, function, file, line);

  nya_todo();
}

void _nya_arena_debug_free(NYA_Arena* arena, const void* ptr, u64 size, const char* function, const char* file, u32 line) {
  unused(arena, ptr, size, function, file, line);

  nya_todo();
}

void _nya_arena_debug_free_all(NYA_Arena* arena, const char* function, const char* file, u32 line) {
  unused(arena, function, file, line);

  nya_todo();
}

void _nya_arena_debug_destroy(NYA_Arena* arena, const char* function, const char* file, u32 line) {
  unused(arena, function, file, line);

  nya_todo();
}

void* _nya_arena_debug_copy(NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line) {
  unused(dst, ptr, size, function, file, line);

  nya_todo();
}

void* _nya_arena_debug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line) {
  unused(src, dst, ptr, size, function, file, line);

  nya_todo();
}

/*
 * ─────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */
