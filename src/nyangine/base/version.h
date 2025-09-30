#pragma once

#if defined(IS_DEBUG) && IS_DEBUG
#define NYA_IS_DEBUG true
#else
#define NYA_IS_DEBUG false
#endif

#ifdef VERSION
#define NYA_VERSION VERSION
#else
#define NYA_VERSION "unknown"
#endif

#ifdef GIT_COMMIT
#define NYA_GIT_COMMIT GIT_COMMIT
#else
#define NYA_GIT_COMMIT "unknown"
#endif
