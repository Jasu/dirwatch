#ifndef DIRWATCH__DETAIL__INOTIFY__OPEN_NODE_VISITOR_H
#define DIRWATCH__DETAIL__INOTIFY__OPEN_NODE_VISITOR_H

#include "../node_visitor.h"

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;
    class open_node_visitor : public ::dirwatch::detail::node_visitor
    {
      public:
        open_node_visitor(::dirwatch::detail::platform_watcher * watcher);
        void visit(::dirwatch::detail::node * n);
      private:
        ::dirwatch::detail::platform_watcher * _platform_watcher;
    };
  }
}

#endif

