#ifndef DIRWATCH__DIRECTORY_WATCHER_H
#define DIRWATCH__DIRECTORY_WATCHER_H

#include <boost/utility.hpp>
#include <boost/filesystem/path.hpp>
#include <vector>

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;
  }
  class event_handler;
  /**
   * @brief Watches a directory and its child nodes.
   */
  class directory_watcher : public boost::noncopyable
  {
    friend class ::dirwatch::detail::platform_watcher;
    public:
      /**
       * @param directory Path to the directory being watched.
       * @param event_mask Which events to process?
       */
      directory_watcher(const ::boost::filesystem::path & directory, unsigned int event_mask = 0xFFFFFFFF);
      ~directory_watcher();

      /**
       * @brief Adds an event handler to the directory watcher.
       *
       * Event handlers must be removed before deleting.
       */
      void add_event_handler(::dirwatch::event_handler * handler);

      /**
       * @brief Removes an event handler from the directory watcher.
       */
      void remove_event_handler(::dirwatch::event_handler * handler);

      /**
       * @brief Blocks and never returns. Calls event handlers.
       */
      void watch();

      /**
       * @brief Gets the path being watched.
       */
      const boost::filesystem::path & get_path() const;
    private:
      /**
       * @addtogroup Functions called by detail::platform_watcher
       * @{
       */
      /**
       * @param target Path relative to _path.
       */
      void on_file_contents_changed(const ::boost::filesystem::path & target);
      /**
       * @param target Path relative to _path.
       */
      void on_node_attributes_changed(const ::boost::filesystem::path & target);
      /**
       * @param target Path relative to _path.
       */
      void on_node_deleted(const ::boost::filesystem::path & target);
      /**
       * @param target Path relative to _path.
       */
      void on_node_created(const ::boost::filesystem::path & target);
      /** @} */

      unsigned int _event_mask;

      ::boost::filesystem::path _path;

      ::dirwatch::detail::platform_watcher * _platform_watcher;

      ::std::vector< ::dirwatch::event_handler*> _event_handlers;
  };
}

#endif

