#ifndef DIRWATCH__DETAIL__KQUEUE__PLATFORM_WATCHER_H
#define DIRWATCH__DETAIL__KQUEUE__PLATFORM_WATCHER_H

#include <boost/filesystem/path.hpp>
#include "../message_creation_visitor.h"
#include "../message_deletion_visitor.h"

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    class node;
    class directory;
    class open_node_visitor;

    class platform_watcher
    {
      friend class ::dirwatch::detail::open_node_visitor;
      public:
        /**
         * @brief Opens the kqueue and starts listening for events.
         */
        platform_watcher(::dirwatch::directory_watcher * watcher);

        /**
         * @brief Closes the kqueue.
         */
        ~platform_watcher();

        /**
         * @brief Scans a directory for any files not present in n and adds them.
         *
         * Also sends addition events.
         */
        void scan_additions(::dirwatch::detail::directory * n);

        void watch();

        const ::boost::filesystem::path & get_path() const;
      private:

        /**
         * @brief KQueue file descriptor.
         */
        int _kqueue;

        ::dirwatch::detail::directory * _root;
        ::dirwatch::directory_watcher * _directory_watcher;

        ::dirwatch::detail::message_creation_visitor _message_creation_visitor;
        ::dirwatch::detail::message_deletion_visitor _message_deletion_visitor;

        /**
         * @brief Gets the file descriptor of the open kqueue.
         */
        int get_kqueue();
    };
  }
}

#endif

