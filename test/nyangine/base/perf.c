#include "nyangine/base/perf.h"

#include "SDL3/SDL_timer.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"

s32 main(void) {
  nya_perf_timer_start("Test Timer");
  SDL_Delay(100);
  nya_perf_timer_stop("Test Timer");

  NYA_PerfMeasurement* measurement = nya_perf_timer_get("Test Timer");
  nya_assert(measurement != nullptr);
  nya_assert(measurement->elapsed >= 100);

  return 0;
}
