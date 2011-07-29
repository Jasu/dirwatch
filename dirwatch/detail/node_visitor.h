#ifndef DIRWATCH__DETAIL__NODE_VISITOR_H
#define DIRWATCH__DETAIL__NODE_VISITOR_H

namespace dirwatch
{
  namespace detail
  {
    class node;

    /**
     * @brief Class for recursively visiting nodes.
     */
    class node_visitor
    {
      public:
        virtual void visit(node * node) = 0;
    };
  }
}

#endif

