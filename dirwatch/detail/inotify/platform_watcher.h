#ifndef DIRWATCH__DETAIL__INOTIFY__PLATFORM_WATCHER_H
#define DIRWATCH__DETAIL__INOTIFY__PLATFORM_WATCHER_H

#include <map>
#include <boost/filesystem/path.hpp>

struct inotify_event;

namespace dirwatch
{
  class directory_watcher;
  namespace detail
  {
    class directory;
    class platform_watcher
    {
      public:
        platform_watcher(::dirwatch::directory_watcher * watcher);
        ~platform_watcher();

        void watch();

        int get_file_descriptor();
        
        const boost::filesystem::path & get_path() const;

        /**
         * @brief Adds a directory to the map of watched directories.
         */
        void add_directory(int watch_descriptor, directory * directory);

        /**
         * @brief Removes a directory from the map of watched directories.
         */
        void remove_directory(int watch_descriptor);
      private:
        /**
         * @brief File descriptor to inotify.
         */
        int _inotify;

        ::dirwatch::directory_watcher * _directory_watcher;

        void handle_event(inotify_event * event);

        ::dirwatch::detail::directory * _directory;

        /**
         * inotify does not allow user-specified tags. Map WD:s to directories.
         */
        ::std::map<int, ::dirwatch::detail::directory*> _watch_descriptor_to_directory;
    };
  }
}

#endif

