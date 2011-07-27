#ifndef DIRWATCH__DETAIL__KQUEUE__NODE_H
#define DIRWATCH__DETAIL__KQUEUE__NODE_H

#include <string>
#include <boost/filesystem/path.hpp>

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;
    class directory;

    class node
    {
      public:
        enum TYPE
        {
          TYPE_FILE,
          TYPE_DIRECTORY
          /**
           * @todo support symlinks
           */
        };

        node(::dirwatch::detail::platform_watcher * watcher, const ::boost::filesystem::path & path, ::dirwatch::detail::directory * parent = NULL);

        virtual ~node();

        void notify_delete(::dirwatch::detail::platform_watcher *watcher);

        /**
         * @brief Returns node type.
         *
         * For node, returns TYPE_FILE.
         */
        virtual ::dirwatch::detail::node::TYPE get_type() const;
        /**
         * @brief Returns parent node or NULL.
         *
         * For the directory being monitored, returns NULL.
         */
        ::dirwatch::detail::directory * get_parent() const;
        /**
         * @brief Returns the file descriptor.
         */
        int get_file_descriptor();
        /**
         * @brief Returns the filename (not full path).
         */
        ::std::string get_filename() const;
        /**
         * @brief Gets the filesystem path relative to the created root node.
         *
         * The created root node is the directory being watched, not necessarily the filesystem root.
         */
        ::boost::filesystem::path get_path() const;
      protected:
        mutable ::dirwatch::detail::directory * _parent;
        int _file_descriptor;
        ::std::string _filename;
    };
  }
}

#endif

