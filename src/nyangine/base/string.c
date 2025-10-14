#include "nyangine/base/string.h"

#include "nyangine/base/array.h"
#include "nyangine/base/assert.h"
#include "nyangine/base/memory.h"
#include "nyangine/base/misc.h"

/*
 * ─────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────
 * */

static char* _nya_strstrn(char* haystack, char* needle, u32 haystack_len, u32 needle_len);

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

__attr_overloaded bool nya_string_contains(const NYA_String* str, const char* substr) {
  nya_assert(str);
  nya_assert(substr);

  return _nya_strstrn((char*)str->items, (char*)substr, str->length, SDL_strlen(substr)) != nullptr;
}

__attr_overloaded bool nya_string_contains(const NYA_String* str, const NYA_String* substr) {
  nya_assert(str);
  nya_assert(substr);

  return _nya_strstrn((char*)str->items, (char*)substr->items, str->length, substr->length) != nullptr;
}

bool nya_string_ends_with(const NYA_String* str, const char* suffix) {
  nya_assert(str);
  nya_assert(suffix);

  u32 str_length    = str->length;
  u32 suffix_length = SDL_strlen(suffix);

  if (str_length < suffix_length) return false;

  return nya_memcmp(str->items + str_length - suffix_length, suffix, suffix_length) == 0;
}

__attr_overloaded bool nya_string_equals(const NYA_String* str1, const char* str2) {
  nya_assert(str1);
  nya_assert(str2);

  u32 str1_length = str1->length;
  u32 str2_length = SDL_strlen(str2);
  if (str1_length != str2_length) return false;

  return nya_memcmp(str1->items, str2, str1_length) == 0;
}

__attr_overloaded bool nya_string_equals(const NYA_String* str1, const NYA_String* str2) {
  nya_assert(str1);
  nya_assert(str2);

  if (str1->length != str2->length) return false;

  return nya_memcmp(str1->items, str2->items, str1->length) == 0;
}

bool nya_string_is_empty(const NYA_String* str) {
  nya_assert(str);

  return str->length == 0;
}

bool nya_string_starts_with(const NYA_String* str, const char* prefix) {
  nya_assert(str);
  nya_assert(prefix);

  u32 str_length    = str->length;
  u32 prefix_length = SDL_strlen(prefix);

  if (str_length < prefix_length) return false;

  return nya_memcmp(str->items, prefix, prefix_length) == 0;
}

NYA_String nya_string_clone(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  NYA_String result = nya_string_new_with_capacity(arena, str->length);
  nya_memcpy(result.items, str->items, str->length);
  result.length = str->length;

  return result;
}

NYA_String nya_string_concat(NYA_Arena* arena, const NYA_String* str1, const NYA_String* str2) {
  nya_assert(arena);
  nya_assert(str1);
  nya_assert(str2);

  NYA_String result = nya_string_new_with_capacity(arena, str1->length + str2->length);
  nya_memcpy(result.items, str1->items, str1->length);
  nya_memcpy(result.items + str1->length, str2->items, str2->length);
  result.length = str1->length + str2->length;

  return result;
}

__attr_overloaded NYA_String nya_string_from(NYA_Arena* arena, const char* cstr) {
  nya_assert(arena);
  nya_assert(cstr);

  u32        length = SDL_strlen(cstr);
  NYA_String result = nya_string_new_with_capacity(arena, length);
  nya_memcpy(result.items, cstr, length);
  result.length = length;

  return result;
}

__attr_overloaded NYA_String nya_string_join(NYA_Arena* arena, const NYA_StringArray* arr, const char* separator) {
  nya_assert(arena);
  nya_assert(arr);
  nya_assert(separator);

  u32 separator_length = SDL_strlen(separator);
  u32 total_length     = 0;

  for (u32 i = 0; i < arr->length; i++) total_length += arr->items[i].length;
  total_length += (arr->length - 1) * separator_length;

  NYA_String result = nya_string_new_with_capacity(arena, total_length);
  for (u64 i = 0; i < arr->length; i++) {
    nya_memmove(result.items + result.length, arr->items[i].items, arr->items[i].length);
    result.length += arr->items[i].length;

    if (i < arr->length - 1) {
      nya_memmove(result.items + result.length, separator, separator_length);
      result.length += separator_length;
    }
  }

  return result;
}

__attr_overloaded NYA_String
nya_string_join(NYA_Arena* arena, const NYA_StringArray* arr, const NYA_String* separator) {
  nya_assert(arena);
  nya_assert(arr);
  nya_assert(separator);

  char* separator_cstr = nya_alloca(separator->length + 1);
  nya_memmove(separator_cstr, separator->items, separator->length);
  separator_cstr[separator->length] = '\0';

  return nya_string_join(arena, arr, separator_cstr);
}

NYA_String nya_string_new(NYA_Arena* arena) {
  nya_assert(arena);

  return (NYA_String)nya_array_new(arena, u8);
}

NYA_String nya_string_new_with_capacity(NYA_Arena* arena, u32 capacity) {
  nya_assert(arena);

  return (NYA_String)nya_array_new_with_capacity(arena, u8, capacity);
}

NYA_String nya_string_sprintf(NYA_Arena* arena, const char* fmt, ...) __attr_fmt_printf(2, 3) {
  nya_assert(arena);
  nya_assert(fmt);

  va_list args;
  va_start(args, fmt);

  u32 length = SDL_vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  NYA_String result = nya_string_new_with_capacity(arena, length);

  va_start(args, fmt);
  (void)SDL_vsnprintf((char*)result.items, length + 1, fmt, args);
  va_end(args);

  result.length = length;

  return result;
}

NYA_String nya_string_substring_excld(NYA_Arena* arena, const NYA_String* str, u32 start, u32 end) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(start <= end);
  nya_assert(end <= str->length);

  u32        length = end - start;
  NYA_String substr = nya_string_new_with_capacity(arena, length);
  nya_memmove(substr.items, str->items + start, length);
  substr.length = length;

  return substr;
}

NYA_String nya_string_substring_incld(NYA_Arena* arena, const NYA_String* str, u32 start, u32 end) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(start <= end);
  nya_assert(end < str->length);

  return nya_string_substring_excld(arena, str, start, end + 1);
}

__attr_overloaded NYA_StringArray nya_string_split(NYA_Arena* arena, const NYA_String* str, const char* separator) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(separator);

  NYA_StringArray result     = nya_array_new(arena, NYA_String);
  u64             sep_length = SDL_strlen(separator);
  u64             start      = 0;
  u64             end        = 0;

  while (end < str->length) {
    if (nya_memcmp(str->items + end, separator, sep_length) == 0) {
      NYA_String substr = nya_string_substring_excld(arena, str, start, end);
      nya_array_push(&result, substr);

      start = end + sep_length;
      end   = start;
    } else {
      end++;
    }
  }

  if (start < str->length) {
    NYA_String substr = nya_string_substring_excld(arena, str, start, end);
    nya_array_push(&result, substr);
  }

  return result;
}

__attr_overloaded NYA_StringArray
nya_string_split(NYA_Arena* arena, const NYA_String* str, const NYA_String* separator) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(separator);

  char* separator_cstr = nya_alloca(separator->length + 1);
  nya_memmove(separator_cstr, separator->items, separator->length);
  separator_cstr[separator->length] = '\0';

  return nya_string_split(arena, str, separator_cstr);
}

NYA_StringArray nya_string_split_lines(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  return nya_string_split(arena, str, "\n");
}

NYA_StringArray nya_string_split_words(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  NYA_StringArray arr    = nya_array_new(arena, NYA_String);
  NYA_String      buffer = nya_string_new(arena);

  nya_array_foreach (str, ch) {
    if (SDL_isspace(ch)) {
      if (!nya_string_is_empty(&buffer)) {
        nya_array_push(&arr, nya_string_clone(arena, &buffer));
        nya_string_clear(&buffer);
      }
      continue;
    }

    nya_array_push(&buffer, ch);
  }

  if (!nya_string_is_empty(&buffer)) nya_array_push(&arr, buffer);

  return arr;
}

__attr_overloaded u32 nya_string_count(const NYA_String* str, const char* substr) {
  nya_assert(str);
  nya_assert(substr);

  u32 count  = 0;
  u32 length = strlen(substr);

  if (nya_unlikely(length == 0 || length > str->length)) return 0;

  for (u32 i = 0; i < str->length; i++) {
    if (i + length > str->length) break;
    if (nya_memcmp(str->items + i, substr, length) == 0) {
      count++;
      i += length - 1;
    }
  }

  return count;
}

__attr_overloaded u32 nya_string_count(const NYA_String* str, const NYA_String* substr) {
  nya_assert(str);
  nya_assert(substr);

  char* substr_cstr = nya_alloca(substr->length + 1);
  nya_memmove(substr_cstr, substr->items, substr->length);
  substr_cstr[substr->length] = '\0';

  return nya_string_count(str, substr_cstr);
}

void nya_string_clear(NYA_String* str) {
  nya_assert(str);

  str->length = 0;
}

__attr_overloaded void nya_string_extend(NYA_String* str, const char* extension) {
  nya_assert(str);
  nya_assert(extension);

  u32 extension_length = SDL_strlen(extension);
  u32 new_length       = str->length + extension_length;

  nya_array_reserve(str, new_length);
  nya_memmove(str->items + str->length, extension, extension_length);
  str->length = new_length;
}

__attr_overloaded void nya_string_extend(NYA_String* str, const NYA_String* extension) {
  nya_assert(str);
  nya_assert(extension);

  u32 new_length = str->length + extension->length;

  nya_array_reserve(str, new_length);
  nya_memmove(str->items + str->length, extension->items, extension->length);
  str->length = new_length;
}

void nya_string_free(NYA_String* str) {
  nya_array_free(str);
}

void nya_string_print(const NYA_String* str) {
  nya_assert(str);

  printf(NYA_FMT_STRING "\n", NYA_FMT_STRING_ARG(*str));
}

__attr_overloaded void nya_string_remove(NYA_String* str, const char* substr) {
  nya_assert(str);
  nya_assert(substr);

  u32 length = SDL_strlen(substr);

  for (u32 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, substr, length) == 0) {
      nya_memmove(str->items + i, str->items + i + length, str->length - i - length);
      str->length -= length;
      i           -= length;
    }
  }
}

__attr_overloaded void nya_string_remove(NYA_String* str, const NYA_String* substr) {
  nya_assert(str);
  nya_assert(substr);

  for (u64 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, substr->items, substr->length) == 0) {
      nya_memmove(str->items + i, str->items + i + substr->length, str->length - i - substr->length);
      str->length -= substr->length;
      i           -= substr->length;
    }
  }
}

__attr_overloaded void nya_string_replace(NYA_String* str, const char* old, const char* new) {
  nya_assert(str);
  nya_assert(old);
  nya_assert(new);

  u32 old_length = strlen(old);
  u32 new_length = strlen(new);

  for (u32 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, old, old_length) == 0) {
      if (old_length != new_length) nya_array_reserve(str, str->length + new_length - old_length);

      nya_memmove(str->items + i + new_length, str->items + i + old_length, str->length - i - old_length);
      nya_memmove(str->items + i, new, new_length);
      str->length += new_length - old_length;
      i           += new_length - 1;
    }
  }
}

__attr_overloaded void nya_string_replace(NYA_String* str, const NYA_String* old, const NYA_String* new) {
  nya_assert(str);
  nya_assert(old);
  nya_assert(new);

  for (u32 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, old->items, old->length) == 0) {
      if (old->length != new->length) nya_array_reserve(str, str->length + new->length - old->length);

      nya_memmove(str->items + i + new->length, str->items + i + old->length, str->length - i - old->length);
      nya_memmove(str->items + i, new->items, new->length);
      str->length += new->length - old->length;
      i           += new->length - 1;
    }
  }
}

void nya_string_reserve(NYA_String* str, u32 capacity) {
  nya_assert(str);

  nya_array_reserve(str, capacity);
}

void nya_string_reverse(NYA_String* str) {
  nya_assert(str);

  nya_array_reverse(str);
}

s32 nya_string_sscanf(NYA_String* str, const char* fmt, ...) __attr_fmt_scanf(2, 3) {
  nya_assert(str);
  nya_assert(fmt);

  va_list args;
  va_start(args, fmt);

  s32 ret = SDL_vsscanf((const char*)str->items, fmt, args);

  va_end(args);
  return ret;
}

void nya_string_strip_prefix(NYA_String* str, const char* prefix) {
  nya_assert(str);
  nya_assert(prefix);

  u32 prefix_length = SDL_strlen(prefix);

  if (nya_memcmp(str->items, prefix, prefix_length) == 0) {
    nya_memmove(str->items, str->items + prefix_length, str->length - prefix_length);
    str->length -= prefix_length;
  }
}

void nya_string_strip_suffix(NYA_String* str, const char* suffix) {
  nya_assert(str);
  nya_assert(suffix);

  u32 suffix_length = SDL_strlen(suffix);

  if (nya_memcmp(str->items + str->length - suffix_length, suffix, suffix_length) == 0) {
    str->length -= suffix_length;
  }
}

void nya_string_to_lower(NYA_String* str) {
  nya_assert(str);

  for (u32 i = 0; i < str->length; i++) str->items[i] = SDL_tolower(str->items[i]);
}

void nya_string_to_upper(NYA_String* str) {
  nya_assert(str);

  for (u32 i = 0; i < str->length; i++) str->items[i] = SDL_toupper(str->items[i]);
}

void nya_string_trim_whitespace(NYA_String* str) {
  nya_assert(str);

  u32 start = 0;
  u32 end   = str->length;

  while (start < str->length && SDL_isspace(str->items[start])) start++;
  while (end > start && SDL_isspace(str->items[end - 1])) end--;

  nya_memmove(str->items, str->items + start, end - start);
  str->length = end - start;
}

/*
 * ─────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

static char* _nya_strstrn(char* haystack, char* needle, u32 haystack_len, u32 needle_len) {
  if (needle_len == 0 || needle_len > haystack_len) return nullptr;

  for (u32 i = 0; i <= haystack_len - needle_len; i++) {
    if (nya_memcmp(haystack + i, needle, needle_len) == 0) return haystack + i;
  }

  return nullptr;
}
