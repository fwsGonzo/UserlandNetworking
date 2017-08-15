cmake_minimum_required (VERSION 3.0.2)
project (userland_tests C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fsanitize=undefined -fsanitize=address")

add_definitions(-DARCH_x86_64 -DOS_TERMINATE_ON_CONTRACT_VIOLATION)
add_definitions(-DARP_PASSTHROUGH)
add_definitions(-DNO_DEBUG)
add_definitions(-DINCLUDEOS_SINGLE_THREADED)

include_directories(network/IncludeOS/api)
include_directories(network/IncludeOS/mod)
include_directories(network/IncludeOS/mod/GSL)
include_directories(pirahna/pirahna/include)

add_subdirectory(network)
add_subdirectory(pirahna)
add_subdirectory(src)

add_dependencies(testsuite network)
add_dependencies(testsuite http_parser)
add_dependencies(testsuite pirahna)
