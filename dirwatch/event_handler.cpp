#include "event_handler.h"

namespace dirwatch
{
  event_handler::event_handler()
  {
  }

  event_handler::~event_handler()
  {
  } 

  void event_handler::on_file_contents_changed(directory_watcher * watcher, const boost::filesystem::path & target)
  {
  }

  void event_handler::on_node_attributes_changed(directory_watcher * watcher, const boost::filesystem::path & target)
  {
  }

  void event_handler::on_node_deleted(directory_watcher * watcher, const boost::filesystem::path & target)
  {
  }

  void event_handler::on_node_created(directory_watcher * watcher, const boost::filesystem::path & target)
  {
  }
}
