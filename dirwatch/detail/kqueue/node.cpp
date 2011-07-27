#include "node.h"
#include "directory.h"
#include "platform_watcher.h"

#include <fcntl.h>


namespace dirwatch
{
  namespace detail
  {
    node::node(platform_watcher * watcher, const boost::filesystem::path & path, directory * parent)
      : _parent(parent),
        _filename(path.filename().string())
    {
      _file_descriptor = open(path.string().c_str(), O_EVTONLY, 0);
      watcher->add_node(this);
    }

    void node::notify_delete(platform_watcher *watcher)
    {
      watcher->trigger_delete_event(this);
    }

    node::~node()
    {
      close(_file_descriptor);
    }

    node::TYPE node::get_type() const
    {
      return node::TYPE_FILE;
    }

    directory * node::get_parent() const
    {
      return _parent;
    }

    int node::get_file_descriptor()
    {
      return _file_descriptor;
    }

    std::string node::get_filename() const
    {
      return _filename;
    }

    boost::filesystem::path node::get_path() const
    {
      if (_parent)
      {
        return _parent->get_path() / _filename;
      }
      else
      {
        //The root is included in the directory watcher
        return "";
      }
    }
  }
}

