#include "nyangine/base/perf.h"

#include <stdio.h>

#include "SDL3/SDL_timer.h"

#include "nyangine/base/ints.h"

s32 main(void) {
  nya_perf_timer_start("Test Timer");

  SDL_Delay(100);

  nya_perf_timer_stop("Test Timer");

  NYA_PerfMeasurementArray* measurements = nya_perf_get_measurements();

  nya_array_foreach (measurements, m) printf("Timer '%s' took " FMTu64 " ms\n", m->name, m->ended - m->started);

  return 0;
}
