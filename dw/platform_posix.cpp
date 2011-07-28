#include "platform.h"

#include <unistd.h>
#include <boost/filesystem/path.hpp>

namespace platform
{
  boost::filesystem::path get_cwd()
  {
    boost::filesystem::path p;
    char * wd = (char*)malloc(4096);
    getcwd(wd, 4096);
    p = wd;
    free(wd);
    return p;
  }
}

