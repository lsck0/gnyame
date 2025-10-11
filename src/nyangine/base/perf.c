#include "nyangine/base/perf.h"

#include "nyangine/base/assert.h"

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

void _nya_perf_timer_start(const char* name) {
  nya_unused(name);
}

void _nya_perf_timer_stop(const char* name) {
  nya_unused(name);
}

void _nya_perf_timer_reset(void) {
  nya_todo();
}

NYA_PerfMeasurementArray* _nya_perf_timer_get_measurements(void) {
  nya_todo();
}
