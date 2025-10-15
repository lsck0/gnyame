#include "nyangine/base/perf.h"

#include "SDL3/SDL_timer.h"

#include "nyangine/base/arena.h"
#include "nyangine/base/array.h"
#include "nyangine/base/assert.h"
#include "nyangine/base/attributes.h"
#include "nyangine/base/logging.h"
#include "nyangine/base/misc.h"
#include "nyangine/base/string.h"

/*
 * ─────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────
 * */

__attr_maybe_unused static NYA_Arena*               _nya_perf_arena        = nullptr;
__attr_maybe_unused static NYA_PerfMeasurementArray _nya_perf_measurements = {0};

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_PerfMeasurement* _nya_perf_timer_get(const char* name) {
  nya_assert(name);

  nya_array_foreach (&_nya_perf_measurements, measurement) {
    if (nya_string_equals(measurement->name, name)) return measurement;
  }

  return nullptr;
}

void _nya_perf_timer_start(const char* name) {
  nya_assert(name);

  NYA_PerfMeasurement new_measurement = {
      .name    = name,
      .started = SDL_GetTicks(),
      .ended   = 0,
      .elapsed = 0,
  };

  NYA_PerfMeasurement* measurement = _nya_perf_timer_get(name);
  if (measurement != nullptr) {
    new_measurement.elapsed = measurement->elapsed;
    *measurement            = new_measurement;
    return;
  }

  nya_array_push(&_nya_perf_measurements, new_measurement);
}

void _nya_perf_timer_stop(const char* name) {
  nya_assert(name);

  NYA_PerfMeasurement* measurement = _nya_perf_timer_get(name);
  nya_assert(measurement != nullptr, "Timer '%s' was not started.", name);
  nya_assert(measurement->ended == 0, "Timer '%s' was already stopped.", name);

  measurement->ended   = SDL_GetTicks();
  measurement->elapsed = measurement->ended - measurement->started;
}

NYA_PerfMeasurementArray* _nya_perf_timer_get_all(void) {
  return &_nya_perf_measurements;
}

/*
 * ─────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

#if NYA_IS_DEBUG
__attr_constructor void _nya_perf_init(void) {
  nya_assert(_nya_perf_arena == nullptr);
  nya_assert(nya_is_zeroed(_nya_perf_measurements));

  _nya_perf_arena        = nya_arena_new();
  _nya_perf_measurements = nya_array_new(_nya_perf_arena, NYA_PerfMeasurement);
}

__attr_destructor void _nya_perf_shutdown(void) {
  nya_arena_destroy(_nya_perf_arena);
  _nya_perf_arena        = nullptr;
  _nya_perf_measurements = (NYA_PerfMeasurementArray){0};
}
#endif // NYA_IS_DEBUG
