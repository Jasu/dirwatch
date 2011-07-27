#ifndef DIRWATCH__DETAIL__DIRECTORY_H
#define DIRWATCH__DETAIL__DIRECTORY_H

#include "node.h"

#include <string>

#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_map.hpp>

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;

    class directory : public node
    {
      public:
        /**
         * @param send_initial_events If true, triggers node added events for each child node scanned.
         *  True when a new directory is added to the directory.
         */
        directory(::dirwatch::detail::platform_watcher * watcher, const ::boost::filesystem::path & path, ::dirwatch::detail::directory * parent = NULL, bool send_initial_events = false);

        /**
         * @brief Removes node from directory.
         *
         * Does not remove anything from disk.
         */
        void remove_node(::dirwatch::detail::platform_watcher *watcher, const ::std::string & filename);

        void notify_delete(::dirwatch::detail::platform_watcher *watcher);

        /**
         * @brief Scans all files in the directory.
         *
         * @param watcher Watcher, which will receive any addition events.
         */
        void scan_additions(::dirwatch::detail::platform_watcher * watcher);

        ::dirwatch::detail::node::TYPE get_type() const;

      private:
        ::boost::ptr_map< ::std::string, ::dirwatch::detail::node> _children;

        /**
         * @param send_initial_events If added node is a directory, passed to its constructor.
         */
        ::dirwatch::detail::node * add_directory_entry(::dirwatch::detail::platform_watcher * watcher, const ::boost::filesystem::directory_entry & entry, bool send_initial_events = false);
    };
  }
}

#endif

