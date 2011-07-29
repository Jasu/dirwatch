#include "close_node_visitor.h"

#include "../node.h"
#include <unistd.h>

namespace dirwatch
{
  namespace detail
  {
    void close_node_visitor::visit(node * node)
    {
      close(node->get_file_descriptor());
    }
  }
}

