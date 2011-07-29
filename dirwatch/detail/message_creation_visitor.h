#ifndef DIRWATCH__DETAIL__MESSAGE_CREATION_VISITOR_H
#define DIRWATCH__DETAIL__MESSAGE_CREATION_VISITOR_H

#include "node_visitor.h"

namespace dirwatch
{
  class directory_watcher;

  namespace detail
  {
    /**
     * @brief Send a creation message on each visited node.
     */
    class message_creation_visitor : public ::dirwatch::detail::node_visitor
    {
      private:
        ::dirwatch::directory_watcher * _watcher;
      public:
        message_creation_visitor(::dirwatch::directory_watcher * watcher);
        void visit(::dirwatch::detail::node * node);
    };
  }
}

#endif

