#ifndef DIRWATCH__DETAIL__DIRECTORY_H
#define DIRWATCH__DETAIL__DIRECTORY_H

#include "node.h"
#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_map.hpp>

namespace dirwatch
{
  namespace detail
  {
    class node_visitor;
    class directory : public ::dirwatch::detail::node
    {
      public:
        /**
         * @brief Opens path and scans it for children recursively, filling _children.
         */
        directory(const ::boost::filesystem::path & path, ::dirwatch::detail::directory * parent = NULL);

        /**
         * @brief Remove child.
         */
        void remove_child(const ::std::string & str);

        /**
         * @return TYPE_DIRECTORY
         */
        ::dirwatch::detail::node::TYPE get_type() const;

        virtual void accept_preorder(::dirwatch::detail::node_visitor * visitor);
        virtual void accept_postorder(::dirwatch::detail::node_visitor * visitor);

        /**
         * @brief Gets child by filename.
         *
         * If not found, returns NULL.
         */
        node * get_child(const ::std::string & filename);

        /**
         * @brief Adds a child to the tree.
         * @return The child node added.
         */
        node * add_child(const ::boost::filesystem::directory_entry & child);
      private:
        ::boost::ptr_map< ::std::string, ::dirwatch::detail::node> _children;
    };
  }
}

#endif

