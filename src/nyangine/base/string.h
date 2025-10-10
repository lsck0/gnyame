#pragma once

#include "nyangine/base/arena.h"
#include "nyangine/base/ints.h"

// use like: nya_debug("string: "NYA_FMT_STRING"\n", NYA_FMT_STRING_ARG(str))
#define NYA_FMT_STRING          "%.*s"
#define NYA_FMT_STRING_ARG(str) (s32)(str).length, (str).items

typedef struct {
  u32        length;
  u32        capacity;
  u8*        items;
  NYA_Arena* arena;
} NYA_String;

typedef struct {
  u32         length;
  u32         capacity;
  NYA_String* items;
  NYA_Arena*  arena;
} NYA_StringArray;

extern bool            nya_string_contains(const NYA_String* str, const char* substr) __attr_overloaded;
extern bool            nya_string_contains(const NYA_String* str, const NYA_String* substr) __attr_overloaded;
extern bool            nya_string_ends_with(const NYA_String* str, const char* suffix);
extern bool            nya_string_equals(const NYA_String* str1, const char* str2) __attr_overloaded;
extern bool            nya_string_equals(const NYA_String* str1, const NYA_String* str2) __attr_overloaded;
extern bool            nya_string_is_empty(const NYA_String* str);
extern bool            nya_string_starts_with(const NYA_String* str, const char* prefix);
extern NYA_String      nya_string_clone(struct NYA_Arena* arena, const NYA_String* str);
extern NYA_String      nya_string_concat(struct NYA_Arena* arena, const NYA_String* str1, const NYA_String* str2);
extern NYA_String      nya_string_from(struct NYA_Arena* arena, const char* cstr) __attr_overloaded;
extern NYA_String      nya_string_join(struct NYA_Arena* arena, const NYA_StringArray* arr, const char* separator) __attr_overloaded;
extern NYA_String      nya_string_join(struct NYA_Arena* arena, const NYA_StringArray* arr, const NYA_String* separator) __attr_overloaded;
extern NYA_String      nya_string_new(struct NYA_Arena* arena);
extern NYA_String      nya_string_new_with_capacity(struct NYA_Arena* arena, u32 capacity);
extern NYA_String      nya_string_sprintf(struct NYA_Arena* arena, const char* fmt, ...) __attr_fmt_printf(2, 3);
extern NYA_String      nya_string_substring_excld(struct NYA_Arena* arena, const NYA_String* str, u32 start, u32 end);
extern NYA_String      nya_string_substring_incld(struct NYA_Arena* arena, const NYA_String* str, u32 start, u32 end);
extern NYA_StringArray nya_string_split(struct NYA_Arena* arena, const NYA_String* str, const char* separator) __attr_overloaded;
extern NYA_StringArray nya_string_split(struct NYA_Arena* arena, const NYA_String* str, const NYA_String* separator) __attr_overloaded;
extern NYA_StringArray nya_string_split_lines(struct NYA_Arena* arena, const NYA_String* str);
extern NYA_StringArray nya_string_split_words(struct NYA_Arena* arena, const NYA_String* str);
extern u32             nya_string_count(const NYA_String* str, const char* substr) __attr_overloaded;
extern u32             nya_string_count(const NYA_String* str, const NYA_String* substr) __attr_overloaded;
extern void            nya_string_clear(NYA_String* str);
extern void            nya_string_extend(NYA_String* str, const char* extension) __attr_overloaded;
extern void            nya_string_extend(NYA_String* str, const NYA_String* extension) __attr_overloaded;
extern void            nya_string_free(NYA_String str);
extern void            nya_string_print(const NYA_String* str);
extern void            nya_string_remove(NYA_String* str, const char* substr) __attr_overloaded;
extern void            nya_string_remove(NYA_String* str, const NYA_String* substr) __attr_overloaded;
extern void            nya_string_replace(NYA_String* str, const char* old, const char* new) __attr_overloaded;
extern void            nya_string_replace(NYA_String* str, const NYA_String* old, const NYA_String* new) __attr_overloaded;
extern void            nya_string_reserve(NYA_String* str, u32 capacity);
extern void            nya_string_reverse(NYA_String* str);
extern s32             nya_string_sscanf(NYA_String* str, const char* fmt, ...) __attr_fmt_printf(2, 3);
extern void            nya_string_strip_prefix(NYA_String* str, const char* prefix);
extern void            nya_string_strip_suffix(NYA_String* str, const char* suffix);
extern void            nya_string_to_lower(NYA_String* str);
extern void            nya_string_to_upper(NYA_String* str);
extern void            nya_string_trim_whitespace(NYA_String* str);
