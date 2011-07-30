#ifndef DIRWATCH__DETAIL
#define DIRWATCH__DETAIL

#include "../node_visitor.h"

namespace dirwatch
{
  namespace detail
  {
    class platform_watcher;
    /**
     * @brief Stops listening to events for visited directories.
     */
    class close_node_visitor : public ::dirwatch::detail::node_visitor
    {
      public:
        close_node_visitor(::dirwatch::detail::platform_watcher * watcher);

        void visit(::dirwatch::detail::node * n);
      private:
        ::dirwatch::detail::platform_watcher * _platform_watcher;
    };
  }
}


#endif

