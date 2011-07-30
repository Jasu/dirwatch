#include "open_node_visitor.h"
#include "../node.h"
#include "../directory.h"
#include "platform_watcher.h"

#include <boost/filesystem/path.hpp>
#include <sys/inotify.h>

namespace dirwatch
{
  namespace detail
  {
    open_node_visitor::open_node_visitor(platform_watcher * watcher)
      : _platform_watcher(watcher)
    {
    }

    void open_node_visitor::visit(node * n)
    {
      n->set_was_content_modified(false);
      n->set_were_attributes_modified(false);
      if (n->get_type() == node::TYPE_DIRECTORY)
      {
        boost::filesystem::path p = _platform_watcher->get_path() / n->get_path();
        directory * d = static_cast<directory*>(n);
        int fd = _platform_watcher->get_file_descriptor();
        uint32_t watch_descriptor = inotify_add_watch(fd, p.string().c_str(), IN_ATTRIB | IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
        d->set_watch_descriptor(watch_descriptor);
        _platform_watcher->add_directory(watch_descriptor, d);
      }
    }
  }
}
