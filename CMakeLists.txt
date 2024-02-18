cmake_minimum_required(VERSION 3.15)
project(WebStable)

set(CXX_STANDARD_REQUIRED 17)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB APP_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.cpp)
file(GLOB HTTP_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/http/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(webstable ${SRC_LIST} ${APP_SRC_LIST} ${HTTP_SRC_LIST})
target_link_libraries(webstable nanonet iohub)
