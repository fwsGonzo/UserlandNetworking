
set(SOURCES
    main.cpp
    usernet.cpp
    os.cpp
    tcp_test1.cpp
  )

add_executable(testsuite ${SOURCES})
add_dependencies(testsuite network)
target_link_libraries(testsuite network rt)

set_target_properties(testsuite PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
