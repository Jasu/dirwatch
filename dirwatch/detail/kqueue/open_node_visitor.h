#ifndef DIRWATCH__DETAIL__KQUEUE__OPEN_NODE_VISITOR_H
#define DIRWATCH__DETAIL__KQUEUE__OPEN_NODE_VISITOR_H

#include "../node_visitor.h"

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;
    class node;

    /**
     * @brief Starts to listen to all opend nodes using kqueue.
     */
    class open_node_visitor : public ::dirwatch::detail::node_visitor
    {
      public:
        open_node_visitor(platform_watcher * watcher);
        void visit(::dirwatch::detail::node * node);
      private:
        platform_watcher * _platform_watcher;
    };
  }
}

#endif

