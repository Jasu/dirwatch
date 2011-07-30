#include "close_node_visitor.h"
#include "platform_watcher.h"
#include "../node.h"
#include "../directory.h"

#include <sys/inotify.h>

namespace dirwatch
{
  namespace detail
  {
    close_node_visitor::close_node_visitor(platform_watcher * watcher)
      : _platform_watcher(watcher)
    {
    }

    void close_node_visitor::visit(node * n)
    {
      if (n->get_type() == node::TYPE_DIRECTORY)
      {
        directory * d = static_cast<directory*>(n);
        uint32_t watch_descriptor = d->get_watch_descriptor();
        inotify_rm_watch(_platform_watcher->get_file_descriptor(), watch_descriptor);
        _platform_watcher->remove_directory(watch_descriptor);
      }
    }
  }
}

