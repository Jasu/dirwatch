#ifndef DIRWATCH__DETAIL__PLATFORM_H
#define DIRWATCH__DETAIL__PLATFORM_H

//Platform macros set in CMakeLists
#ifdef DIRWATCH_PLATFORM_KQUEUE
# include "kqueue/platform_watcher.h"
# include "kqueue/directory.h"
# include "kqueue/node.h"
#elif defined(DIRWATCH_PLATFORM_NULL)
# include "null/platform_watcher.h"
#else
# error "Platform not supported."
#endif

#endif

