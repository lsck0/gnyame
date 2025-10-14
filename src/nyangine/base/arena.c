// TODO: this probably needs yet another rework refactor lmfao... might drop the region list entirely and just have a
// massive single block of reserved memory... also the free list is kinda janky and defrag just doesnt work xD
#include "nyangine/base/arena.h"

#include <stdio.h>

#include "SDL3/SDL_stdinc.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/attributes.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/logging.h"
#include "nyangine/base/memory.h"
#include "nyangine/base/misc.h"

#if defined(__has_feature) && __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
#define ASAN_ENABLED                            true
#define ASAN_PADDING                            64 // bytes
#define asan_poison_memory_region(addr, size)   __asan_poison_memory_region(addr, size)
#define asan_unpoison_memory_region(addr, size) __asan_unpoison_memory_region(addr, size)
static_assert(ASAN_PADDING >= 0);
#else
#define ASAN_ENABLED                            false
#define ASAN_PADDING                            0 // bytes
#define asan_poison_memory_region(addr, size)   ((void)(addr), (void)(size))
#define asan_unpoison_memory_region(addr, size) ((void)(addr), (void)(size))
static_assert(ASAN_PADDING >= 0);
#endif // defined(__has_feature) && __has_feature(address_sanitizer)

#define _NYA_ARENA_DEFRAGMENTATION_THRESHOLD 1000000U // number of frees before defragmentation is triggered

/*
 * ─────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────
 * */

typedef struct NYA_ArenaRegion       NYA_ArenaRegion;
typedef struct NYA_ArenaFreeList     NYA_ArenaFreeList;
typedef struct NYA_ArenaFreeListNode NYA_ArenaFreeListNode;

struct NYA_Arena {
  u32 region_size;
  u8  alignment;

  bool defragmentation_enabled;

  NYA_ArenaRegion *head, *tail;
};

struct NYA_ArenaRegion {
  u32 used;
  u32 capacity;
  u8* memory;

  u32 allocation_counter;

  NYA_ArenaFreeList* free_list;
  NYA_ArenaRegion*   next;
};

// actually a (unbalanced) binary search tree
struct NYA_ArenaFreeList {
  u32 free_bytes;
  u32 free_counter;

  NYA_ArenaFreeListNode *head, *largest;
};

struct NYA_ArenaFreeListNode {
  void* ptr;
  u32   size;

  NYA_ArenaFreeListNode *left, *right;
};

#define _NYA_ARENA_FREE_LIST_NODE_STACK_SIZE 256
static NYA_ArenaFreeListNode* _nya_free_list_node_stack[_NYA_ARENA_FREE_LIST_NODE_STACK_SIZE];

// clang-format off
static void  _nya_arena_align_and_pad_size(NYA_Arena* arena, u32* size);
static void* _nya_arena_free_list_allocate(NYA_ArenaRegion* region, u32 size);
static bool  _nya_arena_free_list_try_extend_allocation(NYA_ArenaRegion* region, void* ptr, u32 size);
static void  _nya_arena_free_list_defragment(NYA_ArenaRegion* region);
static void  _nya_arena_free_list_insert(NYA_ArenaRegion* region, void* ptr, u32 size);
static void  _nya_arena_free_list_remove(NYA_ArenaRegion* region, NYA_ArenaFreeListNode* node, NYA_ArenaFreeListNode* parent);
static void  _nya_arena_free_list_destroy(NYA_ArenaRegion* region);
// clang-format on

/*
 * ─────────────────────────────────────────
 * NON-DEBUG PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_Arena* _nya_arena_nodebug_new_with_options(u8 alignment, u32 region_size, bool defragmentation_enabled) {
  NYA_Arena* arena = nya_malloc(sizeof(NYA_Arena));
  nya_assert(arena != nullptr);

  *arena = (NYA_Arena){
      .region_size             = region_size,
      .alignment               = alignment,
      .defragmentation_enabled = defragmentation_enabled,
      .head                    = nullptr,
      .tail                    = nullptr,
  };

  nya_assert(arena->alignment >= 8);
  nya_assert(arena->region_size % arena->alignment == 0);
  nya_assert(ASAN_PADDING % arena->alignment == 0);

  return arena;
}

NYA_Arena* _nya_arena_nodebug_new(void) {
  return _nya_arena_nodebug_new_with_options(8, U32_MAX - 7, true);
}

void* _nya_arena_nodebug_alloc(NYA_Arena* arena, u32 size) __attr_malloc {
  nya_assert(arena != nullptr);

  if (nya_unlikely(size == 0)) return nullptr;

  _nya_arena_align_and_pad_size(arena, &size);

  if (nya_unlikely(size > arena->region_size)) goto skip_search;

  for (NYA_ArenaRegion* region = arena->head; region != nullptr; region = region->next) {
    // check free list first
    void* ptr = _nya_arena_free_list_allocate(region, size);
    if (ptr != nullptr) {
      if (ASAN_ENABLED) {
        asan_unpoison_memory_region(ptr, size - ASAN_PADDING);
        asan_poison_memory_region((u8*)ptr + size - ASAN_PADDING, ASAN_PADDING);
      }

      region->allocation_counter++;
      return ptr;
    }

    // check if there is space left in the region
    if (region->capacity - region->used >= size) {
      u8* ptr       = region->memory + region->used;
      region->used += size;

      if (ASAN_ENABLED) {
        asan_unpoison_memory_region(ptr, size - ASAN_PADDING);
        asan_poison_memory_region(ptr + size - ASAN_PADDING, ASAN_PADDING);
      }

      region->allocation_counter++;
      return ptr;
    }

    continue;
  }

skip_search:
  // we didnt find a region with enough space or didnt want to search
  u32              new_region_size   = nya_max(size, arena->region_size);
  NYA_ArenaRegion* new_region        = nya_malloc(sizeof(NYA_ArenaRegion));
  u8*              new_region_memory = nya_malloc(new_region_size);
  nya_assert(new_region != nullptr);
  nya_assert(new_region_memory != nullptr);

  *new_region = (NYA_ArenaRegion){
      .used               = size,
      .capacity           = new_region_size,
      .memory             = new_region_memory,
      .allocation_counter = 1,
      .free_list          = nullptr,
      .next               = nullptr,
  };

  if (ASAN_ENABLED) {
    asan_poison_memory_region(new_region->memory, new_region->capacity);
    asan_unpoison_memory_region(new_region->memory, size - ASAN_PADDING);
  }

  if (arena->head == nullptr) {
    arena->head = new_region;
    arena->tail = new_region;
  } else {
    arena->tail->next = new_region;
    arena->tail       = new_region;
  }

  return new_region->memory;
}

void* _nya_arena_nodebug_realloc(NYA_Arena* arena, void* ptr, u32 old_size, u32 new_size) {
  nya_assert(arena != nullptr);

  // edge cases
  if (nya_unlikely(ptr == nullptr)) return nullptr;
  if (nya_unlikely(new_size == old_size)) return ptr;
  if (nya_unlikely(new_size == 0)) {
    nya_arena_free(arena, ptr, old_size);
    return nullptr;
  }

  _nya_arena_align_and_pad_size(arena, &old_size);
  _nya_arena_align_and_pad_size(arena, &new_size);
  u8* old_ptr = (u8*)ptr;

  // realloc to smaller size = partial free
  if (new_size < old_size) {
    // use old memory as the asan padding
    if (ASAN_ENABLED) asan_poison_memory_region(old_ptr + new_size - ASAN_PADDING, ASAN_PADDING);

    nya_arena_free(arena, old_ptr + new_size, old_size - new_size - ASAN_PADDING);

    return old_ptr;
  }

  // find the region
  for (NYA_ArenaRegion* region = arena->head; region; region = region->next) {
    if (!(region->memory <= old_ptr && old_ptr < region->memory + region->capacity)) continue;

    // check if the allocation is followed by big enough freed memory
    if (_nya_arena_free_list_try_extend_allocation(region, old_ptr + old_size, new_size - old_size + ASAN_PADDING)) {
      if (ASAN_ENABLED) asan_unpoison_memory_region(old_ptr, new_size - ASAN_PADDING);

      return old_ptr;
    }

    // if its the last allocation in the region, we can maybe just extend it
    if (old_ptr + old_size == region->memory + region->used &&
        region->used + (new_size - old_size) <= region->capacity) {
      region->used += new_size - old_size;

      if (ASAN_ENABLED) asan_unpoison_memory_region(old_ptr, new_size - ASAN_PADDING);

      return old_ptr;
    }

    // allocate new memory and copy, dont double dipp on the padding
    void* new_ptr = nya_arena_alloc(arena, new_size - ASAN_PADDING);
    nya_memmove(new_ptr, old_ptr, old_size - ASAN_PADDING);
    nya_arena_free(arena, old_ptr, old_size - ASAN_PADDING);

    return new_ptr;
  }

  nya_unreachable();
}

void _nya_arena_nodebug_free(NYA_Arena* arena, void* ptr, u32 size) {
  nya_assert(arena != nullptr);

  if (ptr == nullptr || size == 0) return;

  _nya_arena_align_and_pad_size(arena, &size);
  u8* casted_ptr = (u8*)ptr;

  if (ASAN_ENABLED) asan_poison_memory_region(ptr, size);

  // find the region
  for (NYA_ArenaRegion* region = arena->head; region; region = region->next) {
    if (!(region->memory <= casted_ptr && casted_ptr < region->memory + region->capacity)) continue;

    // last allocation, just move the used pointer back
    if (casted_ptr + size == region->memory + region->used) {
      region->used -= size;
      return;
    }

    // add to free list otherwise
    _nya_arena_free_list_insert(region, ptr, size);

    // maybe defragment
    if (arena->defragmentation_enabled && region->free_list->free_counter >= _NYA_ARENA_DEFRAGMENTATION_THRESHOLD) {
      region->free_list->free_counter = 0;
      _nya_arena_free_list_defragment(region);
    }

    return;
  }

  nya_unreachable();
}

void _nya_arena_nodebug_free_all(NYA_Arena* arena) {
  nya_assert(arena != nullptr);

  for (NYA_ArenaRegion* region = arena->head; region != nullptr; region = region->next) {
    region->used = 0;

    if (ASAN_ENABLED) asan_poison_memory_region(region->memory, region->capacity);

    if (region->free_list != nullptr) _nya_arena_free_list_destroy(region);
  }
}

void _nya_arena_nodebug_garbage_collect(NYA_Arena* arena) {
  nya_assert(arena != nullptr);

  NYA_ArenaRegion* prev = nullptr;
  NYA_ArenaRegion* curr = arena->head;
  NYA_ArenaRegion* next = nullptr;

  while (curr != nullptr) {
    next           = curr->next;
    u32 free_bytes = (curr->free_list != nullptr) ? curr->free_list->free_bytes : 0;

    if (curr->used == 0 || free_bytes == curr->used) {
      if (prev != nullptr) {
        prev->next = next;
      } else {
        arena->head = next;
      }

      if (curr == arena->tail) arena->tail = prev;

      if (curr->free_list != nullptr) _nya_arena_free_list_destroy(curr);
      nya_free(curr->memory);
      nya_free(curr);
    } else {
      prev = curr;
    }

    curr = next;
  }
}

void _nya_arena_nodebug_destroy(NYA_Arena* arena) {
  nya_assert(arena != nullptr);

  for (NYA_ArenaRegion* region = arena->head; region != nullptr;) {
    NYA_ArenaRegion* next = region->next;

    if (region->free_list != nullptr) nya_free(region->free_list);
    nya_free(region->memory);
    nya_free(region);

    region = next;
  }

  nya_free(arena);
}

void* _nya_arena_nodebug_copy(NYA_Arena* dst, void* ptr, u32 size) {
  nya_assert(dst != nullptr);
  if (ptr == nullptr || size == 0) return nullptr;

  void* new_ptr = nya_arena_alloc(dst, size);
  if (!new_ptr) return nullptr;

  nya_memmove(new_ptr, ptr, size);
  return new_ptr;
}

void* _nya_arena_nodebug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u32 size) {
  nya_assert(src != nullptr);
  nya_assert(dst != nullptr);

  if (ptr == nullptr || size == 0) return nullptr;

  void* new_ptr = nya_arena_alloc(dst, size);
  if (!new_ptr) return nullptr;

  nya_memmove(new_ptr, ptr, size);
  nya_arena_free(src, ptr, size);
  return new_ptr;
}

/*
 * ─────────────────────────────────────────
 * DEBUG PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_Arena* _nya_arena_debug_new(const char* function, const char* file, u32 line) {
  // TODO: track allocations
  nya_unused(function, file, line);

  return _nya_arena_nodebug_new();
}

NYA_Arena* _nya_arena_debug_new_with_options(
    u8          alignment,
    u32         region_size,
    bool        defragmentation_enabled,
    const char* function,
    const char* file,
    u32         line
) {
  nya_unused(function, file, line);

  return _nya_arena_nodebug_new_with_options(alignment, region_size, defragmentation_enabled);
}

void* _nya_arena_debug_alloc(NYA_Arena* arena, u32 size, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  return _nya_arena_nodebug_alloc(arena, size);
}

void* _nya_arena_debug_realloc(
    NYA_Arena*  arena,
    void*       ptr,
    u32         old_size,
    u32         new_size,
    const char* function,
    const char* file,
    u32         line
) {
  nya_unused(function, file, line);

  void* new_ptr = _nya_arena_nodebug_realloc(arena, ptr, old_size, new_size);
  if (new_ptr != nullptr && new_size > old_size) nya_memset((u8*)new_ptr + old_size, 0xCD, new_size - old_size);

  return new_ptr;
}

void _nya_arena_debug_free(NYA_Arena* arena, void* ptr, u32 size, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  _nya_arena_nodebug_free(arena, ptr, size);
}

void _nya_arena_debug_free_all(NYA_Arena* arena, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  _nya_arena_nodebug_free_all(arena);
}

void _nya_arena_debug_garbage_collect(NYA_Arena* arena, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  _nya_arena_nodebug_garbage_collect(arena);
}

void _nya_arena_debug_destroy(NYA_Arena* arena, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  _nya_arena_nodebug_destroy(arena);
}

void* _nya_arena_debug_copy(NYA_Arena* dst, void* ptr, u32 size, const char* function, const char* file, u32 line) {
  nya_unused(function, file, line);

  return _nya_arena_nodebug_copy(dst, ptr, size);
}

void* _nya_arena_debug_move(
    NYA_Arena*  src,
    NYA_Arena*  dst,
    void*       ptr,
    u32         size,
    const char* function,
    const char* file,
    u32         line
) {
  nya_unused(function, file, line);

  return _nya_arena_nodebug_move(src, dst, ptr, size);
}

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

u64 nya_arena_memory_usage(NYA_Arena* arena) {
  nya_assert(arena != nullptr);

  u64 total = 0;

  for (NYA_ArenaRegion* region = arena->head; region != nullptr; region = region->next) {
    total += region->used;
    if (region->free_list != nullptr) total -= region->free_list->free_bytes;
  }

  return total;
}

void nya_arena_print(NYA_Arena* arena) {
  nya_assert(arena != nullptr);

  for (NYA_ArenaRegion* region = arena->head; region; region = region->next) {
    printf(" > Region: %p Used: " FMTu32 " / " FMTu32 " \n", (void*)region, region->used, region->capacity);
    if (region->free_list == nullptr) continue;

    u32                    stack_size = 0;
    NYA_ArenaFreeListNode* current    = region->free_list->head;

    printf(" > Free list (total free bytes: " FMTu32 "):\n", region->free_list->free_bytes);
    while (current != nullptr || stack_size > 0) {
      while (current != nullptr) {
        nya_assert(stack_size < _NYA_ARENA_FREE_LIST_NODE_STACK_SIZE, "Free list stack overflow.");
        _nya_free_list_node_stack[stack_size++] = current;
        current                                 = current->left;
      }

      current = _nya_free_list_node_stack[--stack_size];

      printf("   > Free block: %p Size: " FMTu32 "\n", current->ptr, current->size);

      current = current->right;
    }

    if (arena->head == nullptr) printf(" > No regions.\n");
  }
}

/*
 * ─────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

static void _nya_arena_align_and_pad_size(NYA_Arena* arena, u32* size) {
  nya_assert(arena != nullptr);
  nya_assert(size != nullptr);

  if (nya_unlikely(*size == 0)) return;

  *size = ((*size + (arena->alignment - 1)) & ~(arena->alignment - 1)) + ASAN_PADDING;
}

// since the tree is sorted by position, do a full traversal and pick the first fits
static void* _nya_arena_free_list_allocate(NYA_ArenaRegion* region, u32 size) {
  nya_assert(region != nullptr);
  nya_assert(size != 0);

  if (region->free_list == nullptr || region->free_list->largest->size < size) return nullptr;

  u32                    stack_size      = 0;
  NYA_ArenaFreeListNode* current         = region->free_list->head;
  NYA_ArenaFreeListNode* suitable        = nullptr;
  NYA_ArenaFreeListNode* suitable_parent = nullptr;

  while (current != nullptr || stack_size > 0) {
    while (current != nullptr) {
      nya_assert(stack_size < _NYA_ARENA_FREE_LIST_NODE_STACK_SIZE, "Free list stack overflow.");
      _nya_free_list_node_stack[stack_size++] = current;
      current                                 = current->left;
    }

    current = _nya_free_list_node_stack[--stack_size];

    if (current->size >= size) {
      suitable        = current;
      suitable_parent = (stack_size > 0) ? _nya_free_list_node_stack[stack_size - 1] : nullptr;
      break;
    }

    current = current->right;
  }
  nya_assert(suitable != nullptr);
  void* ptr = suitable->ptr;

  // update the node
  suitable->ptr                  = (u8*)suitable->ptr + size;
  suitable->size                -= size;
  region->free_list->free_bytes -= size;
  if (suitable->size == 0) _nya_arena_free_list_remove(region, suitable, suitable_parent);

  return ptr;
}

static bool _nya_arena_free_list_try_extend_allocation(NYA_ArenaRegion* region, void* ptr, u32 size) {
  nya_assert(region != nullptr);
  nya_assert(ptr != nullptr);

  if (region->free_list == nullptr || region->free_list->largest->size < size) return false;

  NYA_ArenaFreeListNode* parent  = nullptr;
  NYA_ArenaFreeListNode* current = region->free_list->head;
  while (current != nullptr) {
    if (ptr < current->ptr) {
      parent  = current;
      current = current->left;
    } else if (ptr > current->ptr) {
      parent  = current;
      current = current->right;
    } else {
      // found it
      if (current->size >= size) {
        current->ptr                   = (u8*)current->ptr + size;
        current->size                 -= size;
        region->free_list->free_bytes -= size;
        if (current->size == 0) _nya_arena_free_list_remove(region, current, parent);

        return true;
      }
      return false;
    }
  }

  return false;
}

static void _nya_arena_free_list_defragment(NYA_ArenaRegion* region) {
  nya_assert(region != NULL);
  nya_assert(region->free_list != NULL);

  if (region->free_list->head == NULL) return;

  nya_todo();
}

static void _nya_arena_free_list_insert(NYA_ArenaRegion* region, void* ptr, u32 size) {
  nya_assert(region != nullptr);
  nya_assert(ptr != nullptr);
  nya_assert(size != 0);

  NYA_ArenaFreeListNode* node = nya_malloc(sizeof(NYA_ArenaFreeListNode));
  nya_assert(node != nullptr);
  *node = (NYA_ArenaFreeListNode){
      .ptr   = ptr,
      .size  = size,
      .left  = nullptr,
      .right = nullptr,
  };

  // check if we need to make a list
  if (region->free_list == nullptr) {
    region->free_list = nya_malloc(sizeof(NYA_ArenaFreeList));
    nya_assert(region->free_list != nullptr);

    *region->free_list = (NYA_ArenaFreeList){
        .free_bytes   = size,
        .head         = node,
        .largest      = node,
        .free_counter = 1,
    };

    return;
  }

  // search for the right place to insert
  NYA_ArenaFreeListNode* parent  = nullptr;
  NYA_ArenaFreeListNode* current = region->free_list->head;
  nya_assert(current != nullptr);

  while (current != nullptr) {
    if (size < current->size) {
      parent  = current;
      current = current->left;
    } else {
      parent  = current;
      current = current->right;
    }
  }
  if (size < parent->size) {
    parent->left = node;
  } else {
    parent->right = node;
  }

  region->free_list->free_bytes += size;
  region->free_list->free_counter++;
  if (region->free_list->largest->size < size) region->free_list->largest = node;
}

static void
_nya_arena_free_list_remove(NYA_ArenaRegion* region, NYA_ArenaFreeListNode* node, NYA_ArenaFreeListNode* parent) {
  nya_assert(region != nullptr);
  nya_assert(region->free_list != nullptr);
  nya_assert(node != nullptr);

  region->free_list->free_bytes -= node->size;
  // recompute largest if needed
  if (region->free_list->largest == node) {
    u32                    stack_size = 0;
    NYA_ArenaFreeListNode* current    = region->free_list->head;
    NYA_ArenaFreeListNode* largest    = nullptr;

    while (current != nullptr || stack_size > 0) {
      while (current != nullptr) {
        nya_assert(stack_size < _NYA_ARENA_FREE_LIST_NODE_STACK_SIZE, "Free list stack overflow.");
        _nya_free_list_node_stack[stack_size++] = current;
        current                                 = current->left;
      }

      current = _nya_free_list_node_stack[--stack_size];

      if (largest == nullptr || current->size > largest->size) largest = current;

      current = current->right;
    }

    region->free_list->largest = largest;
  }

  // leaf node
  if (node->left == nullptr && node->right == nullptr) {
    if (parent == nullptr) {
      region->free_list->head = nullptr;
    } else if (parent->left == node) {
      parent->left = nullptr;
    } else {
      parent->right = nullptr;
    }
    nya_free(node);
    return;
  }

  // one child
  if (node->left == nullptr || node->right == nullptr) {
    NYA_ArenaFreeListNode* child = (node->left != nullptr) ? node->left : node->right;
    if (parent == nullptr) {
      region->free_list->head = child;
    } else if (parent->left == node) {
      parent->left = child;
    } else {
      parent->right = child;
    }
    nya_free(node);
    return;
  }

  // two children, find the smallest in the right subtree
  if (node->left != nullptr && node->right != nullptr) {
    NYA_ArenaFreeListNode* parent_of_smallest = node;
    NYA_ArenaFreeListNode* smallest           = node->right;
    while (smallest->left != nullptr) {
      parent_of_smallest = smallest;
      smallest           = smallest->left;
    }

    // replace node with smallest
    node->ptr  = smallest->ptr;
    node->size = smallest->size;

    // remove smallest from tree
    if (parent_of_smallest->left == smallest) {
      parent_of_smallest->left = smallest->right;
    } else {
      parent_of_smallest->right = smallest->right;
    }
    nya_free(smallest);
    return;
  }

  nya_unreachable();
}

static void _nya_arena_free_list_destroy(NYA_ArenaRegion* region) {
  nya_assert(region != nullptr);
  nya_assert(region->free_list != nullptr);

  u32                    stack_size   = 0;
  NYA_ArenaFreeListNode* current      = region->free_list->head;
  NYA_ArenaFreeListNode* last_visited = nullptr;

  while (current != nullptr || stack_size > 0) {
    while (current != nullptr) {
      nya_assert(stack_size < _NYA_ARENA_FREE_LIST_NODE_STACK_SIZE, "Free list stack overflow.");
      _nya_free_list_node_stack[stack_size++] = current;
      current                                 = current->left;
    }

    NYA_ArenaFreeListNode* peek_node = _nya_free_list_node_stack[stack_size - 1];
    if (peek_node->right != nullptr && last_visited != peek_node->right) {
      current = peek_node->right;
    } else {
      stack_size--;
      last_visited = peek_node;
      nya_free(peek_node);
    }
  }

  nya_free(region->free_list);
  region->free_list = nullptr;
}
