#include "open_node_visitor.h"
#include "platform_watcher.h"
#include "../node.h"
#include "../../exception/exception.h"

#include <fcntl.h>
#include <unistd.h>

//Include kqueue
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace dirwatch
{
  namespace detail
  {
    open_node_visitor::open_node_visitor(platform_watcher * platform_watcher)
      : _platform_watcher(platform_watcher)
    {
    }

    void open_node_visitor::visit(node * node)
    {
      boost::filesystem::path p = _platform_watcher->get_path() / node->get_path();
      int fd = open(p.string().c_str(), O_EVTONLY, 0);
      struct kevent ke;

      node->set_file_descriptor(fd);

      ke.ident = fd;
      ke.filter = EVFILT_VNODE;
      ke.flags = EV_ADD | EV_CLEAR;
      /**
       * @todo Add revoke support.
       */
      ke.fflags = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_RENAME;
      ke.data = NULL;
      ke.udata = node;

      if (kevent(_platform_watcher->get_kqueue(), &ke, 1, NULL, 0, NULL) == -1)
      {
        BOOST_THROW_EXCEPTION(exception::exception() << exception::error_message_info("kevent returned -1 when registering."));
      }
    }
  }
}
