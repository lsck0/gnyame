#pragma once

#if defined(IS_DEBUG) && IS_DEBUG
#define GNY_IS_DEBUG true
#else
#define GNY_IS_DEBUG false
#endif

#ifdef VERSION
#define GNY_VERSION VERSION
#else
#define GNY_VERSION "unknown"
#endif

#ifdef GIT_COMMIT
#define GNY_GIT_COMMIT GIT_COMMIT
#else
#define GNY_GIT_COMMIT "unknown"
#endif
