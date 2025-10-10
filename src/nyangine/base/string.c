#include "nyangine/base/string.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/misc.h"

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

bool nya_string_contains(const NYA_String* str, const char* substr) __attr_overloaded {
  nya_unused(str, substr);

  nya_todo();
}

bool nya_string_contains(const NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_unused(str, substr);

  nya_todo();
}

bool nya_string_ends_with(const NYA_String* str, const char* suffix) {
  nya_unused(str, suffix);

  nya_todo();
}

bool nya_string_equals(const NYA_String* str1, const char* str2) __attr_overloaded {
  nya_unused(str1, str2);

  nya_todo();
}

bool nya_string_equals(const NYA_String* str1, const NYA_String* str2) __attr_overloaded {
  nya_unused(str1, str2);

  nya_todo();
}

bool nya_string_is_empty(const NYA_String* str) {
  nya_unused(str);

  nya_todo();
}

bool nya_string_starts_with(const NYA_String* str, const char* prefix) {
  nya_unused(str, prefix);

  nya_todo();
}

NYA_String nya_string_clone(struct NYA_Arena* arena, const NYA_String* str) {
  nya_unused(arena, str);

  nya_todo();
}

NYA_String nya_string_concat(struct NYA_Arena* arena, const NYA_String* str1, const NYA_String* str2) {
  nya_unused(arena, str1, str2);

  nya_todo();
}

NYA_String nya_string_from(struct NYA_Arena* arena, const char* cstr) __attr_overloaded {
  nya_unused(arena, cstr);

  nya_todo();
}

NYA_String nya_string_join(struct NYA_Arena* arena, const NYA_StringArray* arr, const char* separator) __attr_overloaded {
  nya_unused(arena, arr, separator);

  nya_todo();
}

NYA_String nya_string_join(struct NYA_Arena* arena, const NYA_StringArray* arr, const NYA_String* separator) __attr_overloaded {
  nya_unused(arena, arr, separator);

  nya_todo();
}

NYA_String nya_string_new(struct NYA_Arena* arena) {
  nya_unused(arena);

  nya_todo();
}

NYA_String nya_string_new_with_capacity(struct NYA_Arena* arena, u32 capacity) {
  nya_unused(arena, capacity);

  nya_todo();
}

NYA_String nya_string_sprintf(struct NYA_Arena* arena, const char* fmt, ...) __attr_fmt_printf(2, 3) {
  nya_unused(arena, fmt);

  nya_todo();
}

NYA_String nya_string_substring_excld(struct NYA_Arena* arena, const NYA_String* str, u32 start, u32 end) {
  nya_unused(arena, str, start, end);

  nya_todo();
}

NYA_String nya_string_substring_incld(struct NYA_Arena* arena, const NYA_String* str, u32 start, u32 end) {
  nya_unused(arena, str, start, end);

  nya_todo();
}

NYA_StringArray nya_string_split(struct NYA_Arena* arena, const NYA_String* str, const char* separator) __attr_overloaded {
  nya_unused(arena, str, separator);

  nya_todo();
}

NYA_StringArray nya_string_split(struct NYA_Arena* arena, const NYA_String* str, const NYA_String* separator) __attr_overloaded {
  nya_unused(arena, str, separator);

  nya_todo();
}

NYA_StringArray nya_string_split_lines(struct NYA_Arena* arena, const NYA_String* str) {
  nya_unused(arena, str);

  nya_todo();
}

NYA_StringArray nya_string_split_words(struct NYA_Arena* arena, const NYA_String* str) {
  nya_unused(arena, str);

  nya_todo();
}

u32 nya_string_count(const NYA_String* str, const char* substr) __attr_overloaded {
  nya_unused(str, substr);

  nya_todo();
}

u32 nya_string_count(const NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_unused(str, substr);

  nya_todo();
}

void nya_string_clear(NYA_String* str) {
  nya_unused(str);
}

void nya_string_extend(NYA_String* str, const char* extension) __attr_overloaded {
  nya_unused(str, extension);
}

void nya_string_extend(NYA_String* str, const NYA_String* extension) __attr_overloaded {
  nya_unused(str, extension);
}

void nya_string_free(NYA_String str) {
  nya_unused(str);
}

void nya_string_print(const NYA_String* str) {
  nya_unused(str);
}

void nya_string_remove(NYA_String* str, const char* substr) __attr_overloaded {
  nya_unused(str, substr);
}

void nya_string_remove(NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_unused(str, substr);
}

void nya_string_replace(NYA_String* str, const char* old, const char* new) __attr_overloaded {
  nya_unused(str, old, new);
}

void nya_string_replace(NYA_String* str, const NYA_String* old, const NYA_String* new) __attr_overloaded {
  nya_unused(str, old, new);
}

void nya_string_reserve(NYA_String* str, u32 capacity) {
  nya_unused(str, capacity);
}

void nya_string_reverse(NYA_String* str) {
  nya_unused(str);
}

s32 nya_string_sscanf(NYA_String* str, const char* fmt, ...) __attr_fmt_printf(2, 3) {
  nya_unused(str, fmt);

  nya_todo();
}

void nya_string_strip_prefix(NYA_String* str, const char* prefix) {
  nya_unused(str, prefix);
}

void nya_string_strip_suffix(NYA_String* str, const char* suffix) {
  nya_unused(str, suffix);
}

void nya_string_to_lower(NYA_String* str) {
  nya_unused(str);
}

void nya_string_to_upper(NYA_String* str) {
  nya_unused(str);
}

void nya_string_trim_whitespace(NYA_String* str) {
  nya_unused(str);
}
