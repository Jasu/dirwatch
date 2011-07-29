#include "platform_watcher.h"

#include "../../directory_watcher.h"
#include "../../exception/exception.h"
#include "../node.h"
#include "../directory.h"

#include "open_node_visitor.h"
#include "close_node_visitor.h"
#include "../message_creation_visitor.h"
#include "../message_deletion_visitor.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


namespace dirwatch
{
  namespace detail
  {
    platform_watcher::platform_watcher(directory_watcher * watcher)
      : _directory_watcher(watcher),
        _message_creation_visitor(watcher),
        _message_deletion_visitor(watcher)
    {
      _kqueue = kqueue();
      _root = new directory(watcher->get_path());

      //Open file handles and register them to kqueue.
      open_node_visitor v(this);
      _root->accept_preorder(&v);
    }

    platform_watcher::~platform_watcher()
    {
      //Close file handles
      close_node_visitor v;
      _root->accept_postorder(&v);
      close(_kqueue);
      delete _root;
    }

    void platform_watcher::scan_additions(directory * d)
    {
      boost::filesystem::path p = _directory_watcher->get_path() / d->get_path();
      boost::filesystem::directory_iterator end, it(p);
      for (;it != end; ++it)
      {
        std::string filename = it->path().filename().string();
        if (!d->get_child(filename))
        {
          node * n = d->add_child(*it);
          open_node_visitor v(this);
          n->accept_preorder(&v);
          n->accept_preorder(&_message_creation_visitor);
        }
      }
    }

    void platform_watcher::watch()
    {
      struct kevent ke;
      while (-1 != kevent(_kqueue, NULL, 0, &ke, 1, NULL))
      {
        node * n = reinterpret_cast<node*>(ke.udata);
        boost::filesystem::path p = n->get_path();
        if (ke.fflags & NOTE_DELETE)
        {
          if (n->get_parent())
          {
            n->accept_postorder(&_message_deletion_visitor);
            close_node_visitor v;
            n->accept_postorder(&v);
            n->get_parent()->remove_child(n->get_filename());
          }
        }
        else if (ke.fflags & NOTE_RENAME)
        {
          if (n->get_parent())
          {
            directory * parent = n->get_parent();
            n->accept_postorder(&_message_deletion_visitor);
            close_node_visitor v;
            n->accept_postorder(&v);
            parent->remove_child(n->get_filename());
            //Not sure if the rename event may mean that it was renamed into the same directory?
            scan_additions(parent);
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
              scan_additions(static_cast<directory*>(n));
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

    const boost::filesystem::path & platform_watcher::get_path() const
    {
      return _directory_watcher->get_path();
    }

    int platform_watcher::get_kqueue()
    {
      return _kqueue;
    }
  }
}

