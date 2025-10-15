#pragma once

#include "nyangine/base/logging.h"
#include "nyangine/base/misc.h"

// clang-format off
#define nya_assert(...)                        _NYA_PICK_ASSERT(__VA_ARGS__, _NYA_ASSERT3, _NYA_ASSERT2, _NYA_ASSERT1)(__VA_ARGS__)
#define _NYA_PICK_ASSERT(_1, _2, _3, NAME, ...) NAME
#define _NYA_ASSERT1(condition)                 if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s", #condition); }
#define _NYA_ASSERT2(condition, message)        if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s, %s", #condition, message); }
#define _NYA_ASSERT3(condition, format, ...)    if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s, " format, #condition, ##__VA_ARGS__); }
// clang-format on

// clang-format off
#define nya_todo()          nya_assert(0, "Todo"); __builtin_unreachable();
#define nya_unimplemented() nya_assert(0, "Unimplemented"); __builtin_unreachable();
#define nya_unreachable()   nya_assert(0, "Unreachable"); __builtin_unreachable();
// clang-format on
