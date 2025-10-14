#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"

s32 main(void) {
  typedef enum {
    FLAG_A = 1 << 0,
    FLAG_B = 1 << 1,
    FLAG_C = 1 << 2,
  } SampleFlags;

  SampleFlags flags = FLAG_A;

  nya_flag_set(flags, FLAG_B);
  nya_assert(nya_flag_check(flags, FLAG_A));
  nya_assert(nya_flag_check(flags, FLAG_B));
  nya_assert(!nya_flag_check(flags, FLAG_C));

  nya_flag_unset(flags, FLAG_A);
  nya_assert(!nya_flag_check(flags, FLAG_A));
  nya_assert(nya_flag_check(flags, FLAG_B));
  nya_assert(!nya_flag_check(flags, FLAG_C));

  nya_flag_toggle(flags, FLAG_C);
  nya_assert(!nya_flag_check(flags, FLAG_A));
  nya_assert(nya_flag_check(flags, FLAG_B));
  nya_assert(nya_flag_check(flags, FLAG_C));

  nya_flag_toggle(flags, FLAG_C);
  nya_assert(!nya_flag_check(flags, FLAG_A));
  nya_assert(nya_flag_check(flags, FLAG_B));
  nya_assert(!nya_flag_check(flags, FLAG_C));

  u64 bits = 0x0;
  nya_bit_set(bits, 0);
  nya_bit_set(bits, 1);
  nya_bit_set(bits, 2);
  nya_assert(bits == 0x7);
  nya_bit_toggle(bits, 1);
  nya_assert(bits == 0x5);
  nya_bit_unset(bits, 0);
  nya_assert(bits == 0x4);
  nya_bit_toggle(bits, 0);
  nya_assert(bits == 0x5);
  nya_assert(nya_bit_check(bits, 0));

  nya_assert(nya_min(1, 2) == 1);
  nya_assert(nya_min(2, 1) == 1);
  nya_assert(nya_max(1, 2) == 2);
  nya_assert(nya_max(2, 1) == 2);

  return 0;
}
