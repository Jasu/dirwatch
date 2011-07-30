set(Boost_USE_STATIC_LIBS ON)

# Also search ./lib/boost, so alternative boost may be used.
# Ubuntu ships a too old version of boost as of Natty.
set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0")
#if ("${BOOST_ROOT}" STREQUAL "")
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/lib/boost_1_47_0)
#endif()
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(DW_PLATFORM_SOURCES platform_posix.cpp)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(DW_PLATFORM_SOURCES platform_win32.cpp)
else ()
  message(FATAL_ERROR "Platform not supported.")
endif ()

add_executable(dw dw.cpp ${DW_PLATFORM_SOURCES})

target_link_libraries(dw dirwatch)
target_link_libraries(dw ${Boost_LIBRARIES})

