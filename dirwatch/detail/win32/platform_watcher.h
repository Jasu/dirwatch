#ifndef DIRWATCH__DETAIL__WIN32__PLATFORM_WATCHER_H
#define DIRWATCH__DETAIL__WIN32__PLATFORM_WATCHER_H

#include <boost/filesystem/path.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    class platform_watcher
    {
      public:
        platform_watcher(::dirwatch::directory_watcher * watcher);
        ~platform_watcher();

        void watch();
      private:
        HANDLE _directory_basic;
        HANDLE _directory_attributes;
        /**
         * @brief Overlapped IO event handle for basic file changes.
         */
        HANDLE _ol_event_basic;
        /**
         * @brief Overlapped IO event handle for attributes file changes.
         */
        HANDLE _ol_event_attributes;

        void handle_directory_changes_basic(FILE_NOTIFY_INFORMATION * fni);
        void handle_directory_changes_attributes(FILE_NOTIFY_INFORMATION * fni);

        /**
         * @brief Starts reading basic directory changes.
         *
         * Returns immediately, but ol becomes  active.
         */
        void read_directory_changes_basic(FILE_NOTIFY_INFORMATION * fni, OVERLAPPED * ol);

        /**
         * @brief Starts reading attribute directory changes.
         *
         * Returns immediately, but ol becomes  active.
         */
        void read_directory_changes_attributes(FILE_NOTIFY_INFORMATION * fni, OVERLAPPED * ol);

        ::dirwatch::directory_watcher * _directory_watcher;
    };
  }
}

#endif

