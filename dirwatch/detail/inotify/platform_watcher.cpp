#include "platform_watcher.h"
#include "open_node_visitor.h"
#include "close_node_visitor.h"

#include "../../exception/exception.h"
#include "../directory.h"
#include "../../directory_watcher.h"
#include "../message_deletion_visitor.h"
#include "../message_creation_visitor.h"

#include <sys/inotify.h>
#include <unistd.h>

namespace dirwatch
{
  namespace detail
  {
    platform_watcher::platform_watcher(::dirwatch::directory_watcher * watcher)
      : _directory_watcher(watcher) 
    {
      _inotify = inotify_init();
      _directory = new directory(watcher->get_path());
      open_node_visitor v(this);
      _directory->accept_preorder(&v);
      if (_inotify == -1)
      {
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("Could not initialize inotify."));
      }
    }

    platform_watcher::~platform_watcher()
    {
      close_node_visitor v(this);
      _directory->accept_postorder(&v);
      close(_inotify);
      delete _directory;
    }

    void platform_watcher::watch()
    {
      //VFS limits filesize to 255 bytes + NUL.
      const int buffer_size = 32 * (sizeof(struct inotify_event) + 256);
      char *buffer = (char*)malloc(buffer_size);
      size_t length;
      while ((length = read(_inotify, buffer, buffer_size)) > 0)
      {
        struct inotify_event *event = (inotify_event*)buffer;
        do
        {
          //Assumption: read() does not read partial events.
          if (length < sizeof(inotify_event))
          {
            BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("read a partial event, smaller than sizeof(inotify_event)."));
          }
          if (length < sizeof(inotify_event) + event->len)
          {
            BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("read a partial event, smaller than sizeof(inotify_event) + event->len."));
          }

          handle_event(event);

          length -= sizeof(inotify_event) + event->len;
          event = (inotify_event*)((char*)event + event->len + sizeof(inotify_event));
        }
        while (length);
      }
      BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("read returned value <= 0."));
    }

    void platform_watcher::handle_event(struct inotify_event * event)
    {
      if (event->len)
      {
        std::map<int, directory*>::iterator it = _watch_descriptor_to_directory.find(event->wd);
        //Possible scenario - two events in queue, first being move away, processing it removes the directory.
        if (it == _watch_descriptor_to_directory.end())
          return;
        directory * dir = it->second;
        std::string filename(event->name); //Don't use std::string(event->name, event->len), len includes null bytes.
        boost::filesystem::path p = get_path() / dir->get_path() / filename;
        if (event->mask & IN_ATTRIB)
        {
          node * n = dir->get_child(filename);
          if (n)
          {
            n->set_were_attributes_modified(true);
          }
        }
        if (event->mask & IN_MODIFY)
        {
          node * n = dir->get_child(filename);
          if (n)
          {
            n->set_was_content_modified(true);
          }
        }
        if (event->mask & IN_CLOSE_WRITE)
        {
          node * n = dir->get_child(filename);
          if (n)
          {
            if (n->were_attributes_modified())
            {
              _directory_watcher->on_node_attributes_changed(dir->get_path() / filename);
            }
            if (n->was_content_modified())
            {
              _directory_watcher->on_file_contents_changed(dir->get_path() / filename);
            }
          }
        }
        if (event->mask & IN_CREATE || event->mask & IN_MOVED_TO)
        {
          node * old = dir->get_child(filename);
          if (old)
          {
            //IN_MOVED_TO an existing file
            close_node_visitor cv(this);
            old->accept_postorder(&cv);
            message_deletion_visitor mv(_directory_watcher);
            old->accept_postorder(&mv);
            //Message deletion for the node and all of its child nodes.
            dir->remove_child(filename);
          }
          node * n = dir->add_child(p);
          if (n)
          { //It is possible for n to be null - unsupported file type
            //Message creation for the node and all of its child nodes (in case a whole directory was moved)
            open_node_visitor ov(this);
            n->accept_preorder(&ov);
            message_creation_visitor cv(_directory_watcher);
            n->accept_preorder(&cv);
          }
        }
        if (event->mask & IN_DELETE || event->mask & IN_MOVED_FROM)
        {
          node * n = dir->get_child(filename);
          if (n)
          {
            close_node_visitor cv(this);
            n->accept_postorder(&cv);
            message_deletion_visitor mv(_directory_watcher);
            n->accept_postorder(&mv);
            //Message deletion for the node and all of its child nodes.
            dir->remove_child(filename);
          }
        }
      }
    }

    int platform_watcher::get_file_descriptor()
    {
      return _inotify;
    }

    const boost::filesystem::path & platform_watcher::get_path() const
    {
      return _directory_watcher->get_path();
    }

    void platform_watcher::add_directory(int watch_descriptor, directory * directory)
    {
      _watch_descriptor_to_directory[watch_descriptor] = directory;
    }

    void platform_watcher::remove_directory(int watch_descriptor)
    {
      _watch_descriptor_to_directory.erase(watch_descriptor);
    }
  }
}
