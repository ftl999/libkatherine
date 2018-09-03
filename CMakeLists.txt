cmake_minimum_required(VERSION 3.8)

SET(YAML_PATH "../../yaml-cpp-master/yaml-cpp-master")

PROJECT(libkatherine)
file(GLOB_RECURSE KATHERINE_SOURCES FILES_MATCHING PATTERN "src/*")
file(GLOB_RECURSE KATHERINE_HEADERS FILES_MATCHING PATTERN "include/*")

if(UNIX)
  set (CMAKE_CXX_FLAGS "-std=c++11")
  set (CMAKE_C_FLAGS "-std=c11")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)

add_library(katherine ${KATHERINE_SOURCES} ${KATHERINE_HEADERS})
target_include_directories(katherine PUBLIC include ${YAML_PATH}/include)
target_link_libraries(katherine ${YAML_PATH}/lib/yaml-cpp.lib)