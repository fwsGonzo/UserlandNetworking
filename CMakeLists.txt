cmake_minimum_required (VERSION 2.8.1)
project (userland_tests C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GTK_CFLAGS} -g -Wall -Wextra")

add_definitions(-DARCH_x86_64 -DOS_TERMINATE_ON_CONTRACT_VIOLATION)
add_definitions(-DARP_PASSTHROUGH)
add_definitions(-DNO_DEBUG)

include_directories(network/IncludeOS/api)
include_directories(network/IncludeOS/mod)
include_directories(network/IncludeOS/mod/GSL)

add_subdirectory(network)
add_subdirectory(src)

add_dependencies(testsuite network)
