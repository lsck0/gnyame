#pragma once

#include "nyangine/base/attributes.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/version.h"

typedef struct NYA_Arena NYA_Arena;

#if NYA_IS_DEBUG

#define nya_arena_new                                     _nya_arena_debug_new(__FUNCTION__, __FILE__, __LINE__)
#define nya_arena_alloc(arena, size)                      _nya_arena_debug_alloc(arena, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_realloc(arena, ptr, old_size, new_size) _nya_arena_debug_realloc(arena, ptr, old_size, new_size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_free(arena, ptr, size)                  _nya_arena_debug_free(arena, ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_free_all(arena)                         _nya_arena_debug_free_all(arena, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_destroy(arena)                          _nya_arena_debug_destroy(arena, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_copy(dst, ptr, size)                    _nya_arena_debug_copy(dst, ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_move(src, dst, ptr, size)               _nya_arena_debug_move(src, dst, ptr, size, __FUNCTION__, __FILE__, __LINE__)

// clang-format off
extern NYA_Arena* _nya_arena_debug_new(const char* function, const char* file, u32 line);
extern void*      _nya_arena_debug_alloc(NYA_Arena* arena, u64 size, const char* function, const char* file, u32 line) __attr_malloc;
extern void*      _nya_arena_debug_realloc(NYA_Arena* arena, const void* ptr, u64 old_size, u64 new_size, const char* function, const char* file, u32 line);
extern void       _nya_arena_debug_free(NYA_Arena* arena, const void* ptr, u64 size, const char* function, const char* file, u32 line);
extern void       _nya_arena_debug_free_all(NYA_Arena* arena, const char* function, const char* file, u32 line);
extern void       _nya_arena_debug_destroy(NYA_Arena* arena, const char* function, const char* file, u32 line);
extern void*      _nya_arena_debug_copy(NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line);
extern void*      _nya_arena_debug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line);
// clang-format on

#else

#define nya_arena_new      _nya_arena_nodebug_new
#define nya_arena_alloc    _nya_arena_nodebug_alloc
#define nya_arena_realloc  _nya_arena_nodebug_realloc
#define nya_arena_free     _nya_arena_nodebug_free
#define nya_arena_free_all _nya_arena_nodebug_free_all
#define nya_arena_destroy  _nya_arena_nodebug_destroy
#define nya_arena_copy     _nya_arena_nodebug_copy
#define nya_arena_move     _nya_arena_nodebug_move

extern NYA_Arena* _nya_arena_nodebug_new(void);
extern void*      _nya_arena_nodebug_alloc(NYA_Arena* arena, u64 size) __attr_malloc;
extern void*      _nya_arena_nodebug_realloc(NYA_Arena* arena, const void* ptr, u64 old_size, u64 new_size);
extern void       _nya_arena_nodebug_free(NYA_Arena* arena, const void* ptr, u64 size);
extern void       _nya_arena_nodebug_free_all(NYA_Arena* arena);
extern void       _nya_arena_nodebug_destroy(NYA_Arena* arena);
extern void*      _nya_arena_nodebug_copy(NYA_Arena* dst, void* ptr, u64 size);
extern void*      _nya_arena_nodebug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size);

#endif // NYA_IS_DEBUG
