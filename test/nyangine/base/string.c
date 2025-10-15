#include "nyangine/base/string.h"

#include <stdio.h>

#include "nyangine/base/arena.h"
#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_new();

  // === basic creation ===
  NYA_String s1 = nya_string_from(arena, "Hello, ");
  NYA_String s2 = nya_string_from(arena, "\n\t world!  ");

  // === trim whitespace ===
  nya_string_trim_whitespace(&s2);
  nya_assert(nya_string_equals(&s2, "world!"));

  // === concat ===
  NYA_String s3 = nya_string_concat(arena, &s1, &s2);
  nya_assert(nya_string_equals(&s3, "Hello, world!"));

  // === extend (char*) and (nya_string) ===
  nya_string_extend(&s3, " How are you?");
  nya_assert(nya_string_equals(&s3, "Hello, world! How are you?"));
  NYA_String ex = nya_string_from(arena, " I'm fine.");
  nya_string_extend(&s3, &ex);
  nya_assert(nya_string_contains(&s3, "How are you? I'm fine."));

  // === remove (char*) and (nya_string) ===
  nya_string_remove(&s3, "world! ");
  nya_assert(nya_string_contains(&s3, "Hello, "));
  NYA_String rem = nya_string_from(arena, " I'm fine.");
  nya_string_remove(&s3, &rem);
  nya_assert(!nya_string_contains(&s3, "I'm fine."));

  // === replace (char*) ===
  nya_string_replace(&s3, "How", "Howdy");
  nya_assert(nya_string_contains(&s3, "Howdy"));

  // === reverse ===
  nya_string_reverse(&s3);
  NYA_String rtest = nya_string_from(arena, "abc");
  nya_string_reverse(&rtest);
  nya_assert(nya_string_equals(&rtest, "cba"));

  // === strip prefix / suffix ===
  nya_string_strip_prefix(&s3, "?uoy ");
  NYA_String temp = nya_string_clone(arena, &s3);
  nya_string_reverse(&temp);
  nya_string_strip_prefix(&temp, "Hello");
  nya_string_reverse(&temp);

  // === contains / starts_with / ends_with ===
  NYA_String s4 = nya_string_from(arena, "era ydwoh");
  nya_string_to_lower(&s4);
  nya_assert(nya_string_equals(&s4, "era ydwoh"));
  nya_string_to_upper(&s4);
  nya_assert(nya_string_equals(&s4, "ERA YDWOH"));

  // === clear ===
  nya_string_clear(&s4);
  nya_assert(nya_string_is_empty(&s4));

  // === split / join ===
  NYA_String      s5    = nya_string_from(arena, "Hello, world! How are you?");
  NYA_StringArray parts = nya_string_split(arena, &s5, " ");
  nya_assert(parts.length == 5);
  nya_assert(nya_string_equals(&parts.items[0], "Hello,"));
  nya_assert(nya_string_equals(&parts.items[1], "world!"));
  nya_assert(nya_string_equals(&parts.items[2], "How"));
  nya_assert(nya_string_equals(&parts.items[3], "are"));
  nya_assert(nya_string_equals(&parts.items[4], "you?"));

  NYA_String joined = nya_string_join(arena, &parts, " ");
  nya_assert(nya_string_equals(&joined, &s5));

  // === sprintf ===
  NYA_String s6 = nya_string_sprintf(arena, "Hello, %s! How are you? %.2f", "world", 3.14159);
  nya_assert(nya_string_equals(&s6, "Hello, world! How are you? 3.14"));

  // === sscanf ===
  u32        v1 = 0;
  double     v2 = 0.0;
  NYA_String s7 = nya_string_from(arena, "uwu 42 3.14 a");
  nya_string_sscanf(&s7, "uwu %u %lf a", &v1, &v2);
  nya_assert(v1 == 42);
  nya_assert((f64)(v2 - 3.14) < 1e-9);

  // === count ===
  NYA_String s8 = nya_string_from(arena, "Hello,   world!\nHow are \t\n you?");
  nya_assert(nya_string_count(&s8, "o") == 4);
  nya_assert(nya_string_count(&s8, "world") == 1);

  // === split words and lines ===
  NYA_StringArray words = nya_string_split_words(arena, &s8);
  nya_assert(words.length == 5);
  nya_assert(nya_string_equals(&words.items[0], "Hello,"));
  nya_assert(nya_string_equals(&words.items[1], "world!"));
  nya_assert(nya_string_equals(&words.items[2], "How"));
  nya_assert(nya_string_equals(&words.items[3], "are"));
  nya_assert(nya_string_equals(&words.items[4], "you?"));

  NYA_StringArray lines = nya_string_split_lines(arena, &s8);
  nya_assert(lines.length == 3);
  nya_assert(nya_string_equals(&lines.items[0], "Hello,   world!"));
  nya_assert(nya_string_equals(&lines.items[1], "How are \t"));
  nya_assert(nya_string_equals(&lines.items[2], " you?"));

  // === substring (incld / excld) ===
  NYA_String sub_incld = nya_string_substring_incld(arena, &s5, 0, 4);
  NYA_String sub_excld = nya_string_substring_excld(arena, &s5, 0, 5);
  nya_assert(nya_string_equals(&sub_incld, "Hello"));
  nya_assert(nya_string_equals(&sub_excld, "Hello"));

  // === count (nya_string overload) ===
  NYA_String needle = nya_string_from(arena, "o");
  nya_assert(nya_string_count(&s8, &needle) == 4);

  // === contains overload ===
  nya_assert(nya_string_contains(&s5, "world"));
  nya_assert(nya_string_contains(&s5, &needle));

  // === starts / ends with ===
  nya_assert(nya_string_starts_with(&s5, "Hello"));
  nya_assert(!nya_string_starts_with(&s5, "world"));
  nya_assert(nya_string_ends_with(&s5, "you?"));

  // === clone ===
  NYA_String s5_clone = nya_string_clone(arena, &s5);
  nya_assert(nya_string_equals(&s5_clone, &s5));

  // === new / new_with_capacity / reserve ===
  NYA_String empty = nya_string_new(arena);
  nya_assert(nya_string_is_empty(&empty));
  NYA_String with_cap = nya_string_new_with_capacity(arena, 128);
  nya_string_reserve(&with_cap, 256);
  nya_string_extend(&with_cap, "testing");
  nya_assert(nya_string_contains(&with_cap, "testing"));

  // === replace overload (nya_string) ===
  NYA_String olds     = nya_string_from(arena, "Hello");
  NYA_String news     = nya_string_from(arena, "Hi");
  NYA_String rep_test = nya_string_from(arena, "Hello Hello");
  nya_string_replace(&rep_test, &olds, &news);
  nya_assert(nya_string_count(&rep_test, "Hi") == 2);

  // === remove overload (nya_string) ===
  NYA_String rem_test = nya_string_from(arena, "a b c b a");
  NYA_String torem    = nya_string_from(arena, " b");
  nya_string_remove(&rem_test, &torem);
  nya_assert(!nya_string_contains(&rem_test, " b"));

  // === reverse sanity ===
  NYA_String rev = nya_string_from(arena, "stressed");
  nya_string_reverse(&rev);
  nya_assert(nya_string_equals(&rev, "desserts"));

  // === strip prefix/suffix exact ===
  NYA_String strip_test = nya_string_from(arena, "unwantedHelloWorld!!!");
  nya_string_strip_prefix(&strip_test, "unwanted");
  nya_assert(nya_string_starts_with(&strip_test, "HelloWorld"));
  nya_string_strip_suffix(&strip_test, "!!!");
  nya_assert(!nya_string_ends_with(&strip_test, "!!!"));

  // === cleanup ===
  nya_string_destroy(&s1);
  nya_string_destroy(&s2);
  nya_string_destroy(&s3);
  nya_string_destroy(&s5);
  nya_string_destroy(&s6);
  nya_string_destroy(&s7);
  nya_string_destroy(&s8);
  nya_string_destroy(&s4);
  nya_string_destroy(&s5_clone);
  nya_string_destroy(&empty);
  nya_string_destroy(&with_cap);
  nya_string_destroy(&rep_test);
  nya_string_destroy(&rem_test);
  nya_string_destroy(&rev);
  nya_string_destroy(&strip_test);

  nya_array_destroy(&parts);
  nya_array_destroy(&words);
  nya_array_destroy(&lines);

  nya_arena_garbage_collect(arena);
  nya_arena_destroy(arena);

  return 0;
}
