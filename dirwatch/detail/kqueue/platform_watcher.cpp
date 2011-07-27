#include "platform_watcher.h"

#include "../../directory_watcher.h"
#include "../../exception/exception.h"
#include "node.h"
#include "directory.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


namespace dirwatch
{
  namespace detail
  {
    platform_watcher::platform_watcher(directory_watcher * watcher)
      : _directory_watcher(watcher)
    {
      _kqueue = kqueue();
      _root = new directory(this, watcher->get_path());
    }

    platform_watcher::~platform_watcher()
    {
      delete _root;
      close(_kqueue);
    }

    void platform_watcher::add_node(node * n)
    {
      int fd = n->get_file_descriptor();
      struct kevent ke;

      ke.ident = fd;
      ke.filter = EVFILT_VNODE;
      ke.flags = EV_ADD | EV_CLEAR;
      /**
       * @todo Add revoke support.
       */
      ke.fflags = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_RENAME;
      ke.data = NULL;
      ke.udata = n;

      std::cerr << "Setting udata: " << ke.udata << std::endl;

      if (kevent(_kqueue, &ke, 1, NULL, 0, NULL) == -1)
      {
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("kevent returned -1 when registering."));
      }
    }

    void platform_watcher::watch()
    {
      struct kevent ke;
      while (-1 != kevent(_kqueue, NULL, 0, &ke, 1, NULL))
      {
        std::cerr << "Getting udata: " << ke.udata << std::endl;
        node * n = reinterpret_cast<node*>(ke.udata);
        boost::filesystem::path p = n->get_path();
        if (ke.fflags & NOTE_DELETE)
        {
          if (n->get_parent())
          {
            n->get_parent()->remove_node(this, n->get_filename());
          }
        }
        else if (ke.fflags & NOTE_RENAME)
        {
          if (n->get_parent())
          {
            directory * parent = n->get_parent();
            parent->remove_node(this, n->get_filename());
            parent->scan_additions(this);
          }
        }
        else
        {
          if (ke.fflags & NOTE_WRITE || ke.fflags & NOTE_EXTEND)
          {
            if (n->get_type() == node::TYPE_FILE)
            {
              _directory_watcher->on_file_contents_changed(p);
            }
            else
            {
              static_cast<directory*>(n)->scan_additions(this);
            }
          }
          if (ke.fflags & NOTE_ATTRIB)
          {
            _directory_watcher->on_node_attributes_changed(p);
          }
        }
      }
      BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("kevent returned -1 when watching."));
    }

    void platform_watcher::trigger_add_event(node * n)
    {
      _directory_watcher->on_node_created(n->get_path());
    }

    void platform_watcher::trigger_delete_event(node * n)
    {
      _directory_watcher->on_node_deleted(n->get_path());
    }

    const boost::filesystem::path & platform_watcher::get_path() const
    {
      return _directory_watcher->get_path();
    }
  }
}

