#ifndef DIRWATCH__EXCEPTION__EXCEPTION_H
#define DIRWATCH__EXCEPTION__EXCEPTION_H

#include <exception>
#include <string>
#include <boost/exception/all.hpp>

namespace dirwatch
{
  namespace exception
  {
    typedef ::boost::error_info<class tag_error_message_info, ::std::string> error_message_info;

    struct exception : public virtual ::std::exception, public virtual ::boost::exception
    {
    };
  }
}

#endif

