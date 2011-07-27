set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR})

add_executable(dw dw.cpp)

target_link_libraries(dw dirwatch)
target_link_libraries(dw ${Boost_LIBRARIES})

