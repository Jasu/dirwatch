#include "directory.h"
#include "platform.h"
#include "platform_watcher.h"
#include "../../directory_watcher.h"
#include "../../exception/exception.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace dirwatch
{
  namespace detail
  {
    directory::directory(platform_watcher *watcher, const fs::path & path, directory * parent, bool send_initial_events)
      : node(watcher, path, parent)
    {
      fs::directory_iterator it(path), end; 
      node::TYPE type;
      for (; it != end; it++)
      {
        node * n = add_directory_entry(watcher, *it, send_initial_events);
        if (send_initial_events)
        {
          watcher->trigger_add_event(n);
        }
      }
    }


    void directory::scan_additions(platform_watcher *watcher)
    {
      try
      {
        fs::directory_iterator it(watcher->get_path() / get_path()), end;
        for(;it != end; it++)
        {
          std::string filename = it->path().filename().string();
          if (_children.find(filename) != _children.end())
            continue;
          node * n = add_directory_entry(watcher, *it, true);
          if (n)
          {
            watcher->trigger_add_event(n);
          }
        }
      }
      catch(...)
      {
        //If the file has disappeared, notify of delete.
        if (fs::exists(watcher->get_path() / get_path()))
          throw; //Some other exception
        if (_parent)
        {
          _parent->remove_node(watcher, _filename);
        }
        else
        {
          notify_delete(watcher);
        }
      }
    }

    void directory::notify_delete(platform_watcher *watcher)
    {
      for(boost::ptr_map<std::string, node>::iterator it = _children.begin();
          it != _children.end(); ++it)
      {
        it->second->notify_delete(watcher);
      }
      watcher->trigger_delete_event(this);
    }

    void directory::remove_node(platform_watcher *watcher, const std::string & filename)
    {
      boost::ptr_map<std::string, node>::iterator it = _children.find(filename);
      if (it != _children.end())
      {
        it->second->notify_delete(watcher);
        _children.erase(it);
      }
    }

    node * directory::add_directory_entry(platform_watcher *watcher, const fs::directory_entry & entry, bool send_initial_events)
    {
      const fs::path & subpath = entry.path();
      node * n;
      switch (entry.status().type())
      {
        case fs::regular_file:
        case fs::block_file:
        case fs::character_file:
        case fs::fifo_file:
        case fs::socket_file:
          /**
           * @todo Should socket files etc. be here? Make configurable?
           */
          n = new node(watcher, subpath, this);
          break;
        case fs::directory_file:
          n = new directory(watcher, subpath, this, send_initial_events);
          break;
        case fs::symlink_file:
          /**
           * @todo Handle symlinks? Make configurable?
           */
          return NULL;
        case fs::type_unknown:
          //Skip unknown file types.
          return NULL;
        case fs::status_error:
          BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("boost::filesystem::status_error when iterating."));
        case fs::file_not_found:
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


    node::TYPE directory::get_type() const
    {
      return node::TYPE_DIRECTORY;
    }
  }
}

