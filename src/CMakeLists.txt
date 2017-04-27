
set(SOURCES
    main.cpp
    network.cpp
    usernet.cpp
    os.cpp
    tcp_test1.cpp
    tap_driver.cpp
    tap_device.cpp
  )

add_executable(testsuite ${SOURCES})
add_dependencies(testsuite network)
target_link_libraries(testsuite network http_parser rt)
target_include_directories(testsuite PUBLIC ../libviface/include)

set_target_properties(testsuite PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
