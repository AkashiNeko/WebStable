cmake_minimum_required(VERSION 3.15)
project(WebStable)

set(CXX_STANDARD_REQUIRED 11)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(webstable ${SRC_LIST})
add_library(nanonet STATIC IMPORTED)
add_library(iohub STATIC IMPORTED)
set_target_properties(nanonet PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libnanonet.a)
set_target_properties(iohub PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libiohub.a)
target_link_libraries(webstable nanonet iohub)
