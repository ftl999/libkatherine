file(GLOB_RECURSE KATHERINE_SOURCES FILES_MATCHING PATTERN "src/*")
file(GLOB_RECURSE KATHERINE_HEADERS FILES_MATCHING PATTERN "include/*")

if(UNIX)
  set (CMAKE_CXX_FLAGS "-std=c++11")
  set (CMAKE_C_FLAGS "-std=c11")
endif()

add_library(katherine ${KATHERINE_SOURCES} ${KATHERINE_HEADERS})
target_include_directories(katherine PUBLIC include)
