set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(DW_PLATFORM_SOURCES platform_posix.cpp)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(DW_PLATFORM_SOURCES platform_win32.cpp)
else ()
  message(FATAL_ERROR "Platform not supported.")
endif ()

add_executable(dw dw.cpp ${DW_PLATFORM_SOURCES})

target_link_libraries(dw dirwatch)
target_link_libraries(dw ${Boost_LIBRARIES})

