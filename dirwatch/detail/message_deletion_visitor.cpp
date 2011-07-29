#include "message_deletion_visitor.h"
#include "../directory_watcher.h"
#include "node.h"

namespace dirwatch
{
  namespace detail
  {
    message_deletion_visitor::message_deletion_visitor(directory_watcher * watcher)
      : _watcher(watcher)
    {
    }

    void message_deletion_visitor::visit(node * node)
    {
      _watcher->on_node_deleted(node->get_path());
    }
  }
}

