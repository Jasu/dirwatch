#ifndef DIRWATCH__EVENT_HANDLER_H
#define DIRWATCH__EVENT_HANDLER_H

#include <boost/filesystem/path.hpp>

namespace dirwatch
{
  class directory_watcher;

  /**
   * @brief Abstract base class for all event handlers.
   *
   * Event handler must be explicitly registered to one or more 
   * directory_watchers before events may be received.
   */
  class event_handler
  {
    protected:
      /**
       * @brief Protected, event_handler is abstract.
       */
      event_handler();
    public:
      /**
       * @brief Virtual destructor provided so event_handler* may be deleted.
       */
      virtual ~event_handler();

      /**
       * @brief Called whenever a file within the directory being monitored by directory_watcher has its contents changed.
       *
       * The default implementation does nothing.
       *
       * @param watcher Watcher that sent the message.
       * @param target File whose contents were changed.
       */
      virtual void on_file_contents_changed(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target);

      /**
       * @brief Called whenever a file or directory within the directory being monitored by directory_watcher has its attributes changed.
       *
       * The default implementation does nothing.
       *
       * @param watcher Watcher that sent the message.
       * @param target File whose attributes were changed.
       */
      virtual void on_node_attributes_changed(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target);

      /**
       * @brief Called whenever a file or directory is deleted within the directory being monitored by directory_watcher.
       *
       * The default implementation does nothing.
       *
       * @param watcher Watcher that sent the message.
       * @param target File which was deleted.
       *
       * @note File move or rename is presented as a delete followed by create on all platforms.
       */
      virtual void on_node_deleted(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target);

      /**
       * @brief Called whenever a file or directory is created within the directory being monitored by directory_watcher.
       *
       * The default implementation does nothing.
       *
       * @param watcher Watcher that sent the message.
       * @param target File which was created.
       *
       * @note File move or rename is presented as a delete followed by create on all platforms.
       */
      virtual void on_node_created(::dirwatch::directory_watcher * watcher, const ::boost::filesystem::path & target);
  };
}

#endif

