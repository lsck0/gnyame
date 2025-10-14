#pragma once

#include "nyangine/base/array.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/misc.h"
#include "nyangine/base/version.h"

typedef struct {
  const char* name;
  u64         started;
  u64         ended;
} NYA_PerfMeasurement;
nya_derive_array(NYA_PerfMeasurement);

#if NYA_IS_DEBUG

#define nya_perf_timer_get(name)   _nya_perf_timer_get(name)
#define nya_perf_timer_start(name) _nya_perf_timer_start(name)
#define nya_perf_timer_stop(name)  _nya_perf_timer_stop(name)
#define nya_perf_timer_reset(name) _nya_perf_timer_reset(name)
#define nya_perf_timer_get_all()   _nya_perf_timer_get_all()

extern NYA_PerfMeasurement*      _nya_perf_timer_get(const char* name);
extern void                      _nya_perf_timer_start(const char* name);
extern void                      _nya_perf_timer_stop(const char* name);
extern void                      _nya_perf_timer_reset(const char* name);
extern NYA_PerfMeasurementArray* _nya_perf_timer_get_all(void);

#else

// clang-format off
#define nya_perf_get_timer(name)    ({ nya_unused(name); nullptr; })
#define nya_perf_timer_start(name) nya_unused(name)
#define nya_perf_timer_stop(name)  nya_unused(name)
#define nya_perf_timer_reset(name) nya_unused(name)
#define nya_perf_timer_get_all()   nullptr
// clang-format on

#endif // NYA_IS_DEBUG
