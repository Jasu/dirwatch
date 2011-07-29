#include "message_creation_visitor.h"
#include "../directory_watcher.h"
#include "node.h"

namespace dirwatch
{
  namespace detail
  {
    message_creation_visitor::message_creation_visitor(directory_watcher * watcher)
      : _watcher(watcher)
    {
    }

    void message_creation_visitor::visit(node * node)
    {
      _watcher->on_node_created(node->get_path());
    }
  }
}

