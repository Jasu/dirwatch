#include "directory.h"
#include "node_visitor.h"
#include "../exception/exception.h"
#include <boost/filesystem.hpp>

namespace dirwatch
{
  namespace detail
  {
    directory::directory(const boost::filesystem::path & path, ::dirwatch::detail::directory * parent)
      : node(path, parent)
    {
      boost::filesystem::directory_iterator it(path), end; 
      for (; it != end; it++)
      {
        node * n = add_child(*it);
      }
    }

    void directory::remove_child(const std::string & str)
    {
      _children.erase(str);
    }

    node::TYPE directory::get_type() const
    {
      return node::TYPE_DIRECTORY;
    }

    void directory::accept_preorder(node_visitor * visitor)
    {
      node::accept_preorder(visitor);
      for (boost::ptr_map<std::string, node>::iterator it = _children.begin();
          it != _children.end(); ++it)
      {
        it->second->accept_preorder(visitor);
      }
    }

    void directory::accept_postorder(node_visitor * visitor)
    {
      for (boost::ptr_map<std::string, node>::iterator it = _children.begin();
          it != _children.end(); ++it)
      {
        it->second->accept_postorder(visitor);
      }
      node::accept_postorder(visitor);
    }

    node * directory::get_child(const ::std::string & filename)
    {
      boost::ptr_map<std::string, node>::iterator it = _children.find(filename);
      if (it == _children.end())
        return NULL;
      return it->second;
    }

    node * directory::add_child(const boost::filesystem::path & path)
    {
      return add_child(boost::filesystem::directory_entry(path));
    }

    node * directory::add_child(const boost::filesystem::directory_entry & entry)
    {
      const boost::filesystem::path & subpath = entry.path();
      node * n;
      switch (entry.status().type())
      {
        case boost::filesystem::regular_file:
        case boost::filesystem::block_file:
        case boost::filesystem::character_file:
        case boost::filesystem::fifo_file:
        case boost::filesystem::socket_file:
          /**
           * @todo Should socket files etc. be here? Make configurable?
           */
          n = new node(subpath, this);
          break;
        case boost::filesystem::directory_file:
          n = new directory(subpath, this);
          break;
        case boost::filesystem::symlink_file:
          /**
           * @todo Handle symlinks? Make configurable?
           */
          return NULL;
        case boost::filesystem::type_unknown:
          //Skip unknown file types.
          return NULL;
        case boost::filesystem::status_error:
          BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("boost::filesystem::status_error when iterating."));
        case boost::filesystem::file_not_found:
          //Maybe some file which was briefly there.
          return NULL;
          //BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("boost::filesystem::file_not_found when iterating."));
        default:
          BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Unknown file type when iterating."));
      }
      std::string filename = subpath.filename().string();
      _children.insert(filename, n);
      return n;
    }

#ifdef DIRWATCH_PLATFORM_INOTIFY
    int directory::get_watch_descriptor()
    {
      return _watch_descriptor;
    }

    void directory::set_watch_descriptor(int wd)
    {
      _watch_descriptor = wd;
    }
#endif
  }
}

