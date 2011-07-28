cmake_minimum_required(VERSION 2.4)
 
project(dirwatch)

if ("${DIRWATCH_PLATFORM}" STREQUAL "null")
  set(DIRWATCH_PLATFORM_SOURCES 
    detail/null/platform_watcher.cpp
    )
  set(DIRWATCH_PLATFORM_NAME NULL)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(DIRWATCH_PLATFORM_SOURCES 
    detail/kqueue/node.cpp
    detail/kqueue/directory.cpp
    detail/kqueue/platform_watcher.cpp
    )
  set(DIRWATCH_PLATFORM_NAME KQUEUE)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(DIRWATCH_PLATFORM_SOURCES 
    detail/win32/platform_watcher.cpp
    )
  set(DIRWATCH_PLATFORM_NAME WIN32)
else ()
  message(FATAL_ERROR "Platform not supported.")
endif ()
  

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

add_library(
  dirwatch STATIC

  directory_watcher.cpp
  event_handler.cpp

  ${DIRWATCH_PLATFORM_SOURCES}
)

target_link_libraries(dirwatch ${Boost_LIBRARIES})

set_property(
  TARGET dirwatch
  PROPERTY COMPILE_DEFINITIONS DIRWATCH_PLATFORM_${DIRWATCH_PLATFORM_NAME}=1
  )



