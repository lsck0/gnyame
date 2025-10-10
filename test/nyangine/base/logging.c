#include "nyangine/base.h"

s32 main(void) {
  nya_log_level_set(NYA_LOG_LEVEL_DEBUG);

  nya_debug("Hello");
  nya_debug("Hello, %s %d!", "world", 42);

  nya_log_level_set(NYA_LOG_LEVEL_INFO);

  nya_debug("you should not see this");

  return 0;
}
