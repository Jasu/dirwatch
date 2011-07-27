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
else ()
  message(FATAL_ERROR "Platform not supported.")
endif ()
  

option(DIRWATCH_IS_SHARED "Compile as dynamic library/shared object?" ON)

if (${DIRWATCH_IS_SHARED} STREQUAL "ON")
  set (DIRWATCH_LINKAGE SHARED)
else ()
  set (DIRWATCH_LINKAGE STATIC)
endif ()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

add_library(
  dirwatch ${DIRWATCH_LINKAGE}

  directory_watcher.cpp
  event_handler.cpp

  ${DIRWATCH_PLATFORM_SOURCES}
)

target_link_libraries(dirwatch ${Boost_LIBRARIES})

set_property(
  TARGET dirwatch
  PROPERTY COMPILE_DEFINITIONS DIRWATCH_PLATFORM_${DIRWATCH_PLATFORM_NAME}=1
  )



