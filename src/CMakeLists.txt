
set(SOURCES
    main.cpp
    nic.cpp
    os.cpp

  )

add_executable(testsuite ${SOURCES})
add_dependencies(testsuite network)
target_link_libraries(testsuite network)
#target_include_directories(testsuite ${CMAKE_CURRENT_SOURCE_DIR}/IncludeOS/api)
