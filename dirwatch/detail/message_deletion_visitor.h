#ifndef DIRWATCH__DETAIL__MESSAGE_DELETION_VISITOR_H
#define DIRWATCH__DETAIL__MESSAGE_DELETION_VISITOR_H

#include "node_visitor.h"

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    class node;
    /**
     * @brief Sends a deletion message on each visited node.
     */
    class message_deletion_visitor : public ::dirwatch::detail::node_visitor
    {
      private:
        ::dirwatch::directory_watcher * _watcher;
      public:
        message_deletion_visitor(::dirwatch::directory_watcher * watcher);
        void visit(::dirwatch::detail::node * node);
    };
  }
}

#endif

