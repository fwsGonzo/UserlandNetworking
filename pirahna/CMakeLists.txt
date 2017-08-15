
set(LB_SOURCES
  pirahna/src/algorithm.cpp
  pirahna/src/commander.cpp
  pirahna/src/config.cpp
  pirahna/src/fleet.cpp
  pirahna/src/link.cpp
  pirahna/src/radio.cpp
  pirahna/src/uplink.cpp
)

add_library(pirahna STATIC ${LB_SOURCES})
target_include_directories(pirahna PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pirahna/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../network/IncludeOS/mod/rapidjson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../network/IncludeOS/lib/mana/include)
