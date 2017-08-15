
set(SOURCES
    main.cpp
    os.cpp
    network.cpp
    drivers/usernet.cpp
    drivers/tap_driver.cpp
    drivers/memdisk.cpp
    test_system.cpp
    http.cpp
    websocket.cpp
    common_services.cpp
    acorn.cpp
    pirahna.cpp

    fuzz_ip4.cpp

    tcp_test1.cpp
    tap_device.cpp
    stdin_device.cpp
    random_fuzzing.cpp
  )

add_executable(testsuite ${SOURCES})
add_dependencies(testsuite network)
target_link_libraries(testsuite network http_parser rt pirahna)
target_include_directories(testsuite PUBLIC acorn)
target_include_directories(testsuite PUBLIC ../libviface/include)
target_include_directories(testsuite PUBLIC ../network/IncludeOS/examples/acorn/app)
target_include_directories(testsuite PUBLIC ../network/IncludeOS/lib/mana/include)

set_target_properties(testsuite PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
