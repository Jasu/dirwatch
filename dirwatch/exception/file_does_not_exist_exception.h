#ifndef DIRWATCH__EXCEPTION__FILE_DOES_NOT_EXIST_EXCEPTION_H
#define DIRWATCH__EXCEPTION__FILE_DOES_NOT_EXIST_EXCEPTION_H

#include "exception.h"
#include <boost/filesystem/path.hpp>

namespace dirwatch
{
  namespace exception
  {
    
    typedef ::boost::error_info<class tag_error_filename_info, ::boost::filesystem::path> error_filename_info;

    struct file_does_not_exist_exception : public ::dirwatch::exception::exception
    { 
    };
  }
}

#endif

