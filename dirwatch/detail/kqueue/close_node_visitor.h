#ifndef DIRWATCH__DETAIL__KQUEUE__CLOSE_NODE_VISITOR_H
#define DIRWATCH__DETAIL__KQUEUE__CLOSE_NODE_VISITOR_H

#include "../node_visitor.h"

namespace dirwatch
{
  namespace detail
  {
    /**
     * @brief Close the file descriptor associated with each node.
     */
    class close_node_visitor : public ::dirwatch::detail::node_visitor
    {
      public:
        void visit(::dirwatch::detail::node * node);
    };
  }
}

#endif

