#ifndef DIRWATCH__DETAIL__KQUEUE__NODE_H
#define DIRWATCH__DETAIL__KQUEUE__NODE_H

#include <string>
#include <boost/filesystem/path.hpp>

namespace dirwatch
{
  namespace detail
  {
    class node_visitor;
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
      protected:
        mutable ::dirwatch::detail::directory * _parent;
        ::std::string _filename;
      public:

        node(const ::boost::filesystem::path & path, ::dirwatch::detail::directory * parent = NULL);

        virtual ~node();

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
         * @brief Returns the filename (not full path).
         */
        ::std::string get_filename() const;
        /**
         * @brief Gets the filesystem path relative to the created root node.
         *
         * The created root node is the directory being watched, not necessarily the filesystem root.
         */
        ::boost::filesystem::path get_path() const;

        /**
         * @brief Visits node and all the descendants.
         */
        virtual void accept_preorder(::dirwatch::detail::node_visitor * visitor);
        /**
         * @brief Visits all descendants and the node.
         */
        virtual void accept_postorder(::dirwatch::detail::node_visitor * visitor);

#ifdef DIRWATCH_PLATFORM_KQUEUE
      public:
        void set_file_descriptor(int fd);
        int get_file_descriptor();
      private:
        int _file_descriptor;
#elif defined(DIRWATCH_PLATFORM_INOTIFY)
      public:
        //inotify sends 2 kinds of events: 
        // - File modified on each write
        // - File attributes changed on each attr change
        // - File write close when closed for writing.
        //
        // To match semantics of other platforms,
        // only send events when the file is closed.
        bool was_content_modified();
        void set_was_content_modified(bool was_modified);

        bool were_attributes_modified();
        void set_were_attributes_modified(bool were_attributes_modified);
      private:
        bool _was_content_modified;
        bool _were_attributes_modified;
#endif
    };
  }
}

#endif

