#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace platform
{
  boost::filesystem::path get_cwd()
  {
    boost::filesystem::path p;
    wchar_t * wd = (wchar_t*)malloc((MAX_PATH+1)*sizeof(wchar_t));
    GetCurrentDirectoryW(MAX_PATH+1, wd);
    p = wd;
    free(wd);
    return p;
  }
}

