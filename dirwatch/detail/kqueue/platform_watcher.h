#ifndef DIRWATCH__DETAIL__KQUEUE__PLATFORM_WATCHER_H
#define DIRWATCH__DETAIL__KQUEUE__PLATFORM_WATCHER_H

#include <boost/filesystem/path.hpp>

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    class node;
    class directory;

    class platform_watcher
    {
      public:
        platform_watcher(::dirwatch::directory_watcher * watcher);
        ~platform_watcher();

        /**
         * @brief Starts listening for events on node.
         *
         * When the file descriptor of n is closed, the event is automatically removed by kqueue.
         */
        void add_node(::dirwatch::detail::node * n);
        void watch();
        void trigger_add_event(::dirwatch::detail::node * n);
        void trigger_delete_event(::dirwatch::detail::node * n);
        const ::boost::filesystem::path & get_path() const;
      private:

        /**
         * @brief KQueue file descriptor.
         */
        int _kqueue;

        ::dirwatch::detail::directory * _root;
        ::dirwatch::directory_watcher * _directory_watcher;
    };
  }
}

#endif

