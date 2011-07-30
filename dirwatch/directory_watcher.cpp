#include "directory_watcher.h"
#include "event_mask.h"
#include "event_handler.h"
#include "detail/platform_watcher.h"

namespace dirwatch
{
  directory_watcher::directory_watcher(const boost::filesystem::path & directory, unsigned int event_mask)
    : _event_mask(event_mask),
      _path(directory)
  {
    _platform_watcher = new detail::platform_watcher(this);
  }

  directory_watcher::~directory_watcher()
  {
    delete _platform_watcher;
  }

  void directory_watcher::add_event_handler(event_handler * handler)
  {
    _event_handlers.push_back(handler);
  }

  void directory_watcher::remove_event_handler(event_handler * handler)
  {
    for (std::vector<event_handler*>::iterator it = _event_handlers.begin();
        it != _event_handlers.end(); ++it)
    {
      if (*it == handler)
      {
        _event_handlers.erase(it);
        return;
      }
    }
  }

  void directory_watcher::watch()
  {
    _platform_watcher->watch();
  }

  const boost::filesystem::path & directory_watcher::get_path() const
  {
    return _path;
  }

  void directory_watcher::on_file_contents_changed(const ::boost::filesystem::path & target)
  {
    if (!(_event_mask & event_mask::file_contents_changed))
      return;

    for (std::vector<event_handler*>::iterator it = _event_handlers.begin();
        it != _event_handlers.end(); ++it)
    {
      (*it)->on_file_contents_changed(this, _path / target);
    }
  }

  void directory_watcher::on_node_attributes_changed(const ::boost::filesystem::path & target)
  {
    if (!(_event_mask & event_mask::node_attributes_changed))
      return;

    for (std::vector<event_handler*>::iterator it = _event_handlers.begin();
        it != _event_handlers.end(); ++it)
    {
      (*it)->on_node_attributes_changed(this, _path / target);
    }
  }

  void directory_watcher::on_node_deleted(const ::boost::filesystem::path & target)
  {
    if (!(_event_mask & event_mask::node_deleted))
      return;

    for (std::vector<event_handler*>::iterator it = _event_handlers.begin();
        it != _event_handlers.end(); ++it)
    {
      (*it)->on_node_deleted(this, _path / target);
    }
  }

  void directory_watcher::on_node_created(const ::boost::filesystem::path & target)
  {
    if (!(_event_mask & event_mask::node_created))
      return;

    for (std::vector<event_handler*>::iterator it = _event_handlers.begin();
        it != _event_handlers.end(); ++it)
    {
      (*it)->on_node_created(this, _path / target);
    }
  }
}

