
set(SOURCES
    main.cpp
    os.cpp
    network.cpp
    drivers/usernet.cpp
    drivers/tap_driver.cpp
    test_system.cpp
    http.cpp
    websocket.cpp

    tcp_test1.cpp
    tap_device.cpp
    stdin_device.cpp
    random_fuzzing.cpp
  )

add_executable(testsuite ${SOURCES})
add_dependencies(testsuite network)
target_link_libraries(testsuite network http_parser rt)
target_include_directories(testsuite PUBLIC ../libviface/include)

set_target_properties(testsuite PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
