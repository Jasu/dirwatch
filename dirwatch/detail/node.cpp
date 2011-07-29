#include "node.h"

#include "directory.h"
#include "node_visitor.h"

namespace dirwatch
{
  namespace detail
  {
    node::node(const boost::filesystem::path & path, directory * parent)
      : _parent(parent),
        _filename(path.filename().string())
    {
    }
 
    node::~node()
    {
    }

    node::TYPE node::get_type() const
    {
      return node::TYPE_FILE;
    }

    directory * node::get_parent() const
    {
      return _parent;
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

    void node::accept_preorder(::dirwatch::detail::node_visitor * visitor)
    {
      visitor->visit(this);
    }

    void node::accept_postorder(::dirwatch::detail::node_visitor * visitor)
    {
      visitor->visit(this);
    }

#ifdef DIRWATCH_PLATFORM_KQUEUE
    void node::set_file_descriptor(int fd)
    {
      _file_descriptor = fd;
    }

    int node::get_file_descriptor()
    {
      return _file_descriptor;
    }
#endif
  }
}

