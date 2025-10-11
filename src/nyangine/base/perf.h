#pragma once

#include "nyangine/base/array.h"
#include "nyangine/base/ints.h"
#include "nyangine/base/misc.h"
#include "nyangine/base/version.h"

typedef struct {
  const char* name;
  f32         started;
  f32         ended;
} NYA_PerfMeasurement;
nya_derive_array(NYA_PerfMeasurement);

#if NYA_IS_DEBUG

#define nya_perf_timer_start(name)        _nya_perf_timer_start(name)
#define nya_perf_timer_stop(name)         _nya_perf_timer_stop(name)
#define nya_perf_timer_reset()            _nya_perf_timer_reset()
#define nya_perf_timer_get_measurements() _nya_perf_timer_get_measurements()

extern void                      _nya_perf_timer_start(const char* name);
extern void                      _nya_perf_timer_stop(const char* name);
extern void                      _nya_perf_timer_reset(void);
extern NYA_PerfMeasurementArray* _nya_perf_timer_get_measurements(void);

#else

#define nya_perf_timer_start(name)        nya_unused(name)
#define nya_perf_timer_stop(name)         nya_unused(name)
#define nya_perf_timer_reset()            nya_unused()
#define nya_perf_timer_get_measurements() nullptr

#endif // NYA_IS_DEBUG
