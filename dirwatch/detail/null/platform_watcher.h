#ifndef DIRWATCH__DETAIL__NULL__PLATFORM_WATCHER_H
#define DIRWATCH__DETAIL__NULL__PLATFORM_WATCHER_H

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    class platform_watcher
    {
      public:
        platform_watcher(::dirwatch::directory_watcher * dirwatch);

        /**
         * @brief Spins in a busyloop.
         */
        void watch();
    };
  }
}

#endif

