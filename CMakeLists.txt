cmake_minimum_required(VERSION 3.15)
project(WebStable)

set(CXX_STANDARD_REQUIRED 17)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(webstable ${SRC_LIST})
target_link_libraries(webstable nanonet iohub)
