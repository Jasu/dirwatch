#ifndef DW__PLATFORM_H
#define DW__PLATFORM_H

#include <boost/filesystem/path.hpp>

namespace platform
{
  /**
   * @brief Gets the current working directory.
   */
  boost::filesystem::path get_cwd();
}

#endif

