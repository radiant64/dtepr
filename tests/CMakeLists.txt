add_subdirectory(lib)

file(GLOB SOURCES src/*.c src/*.h)

add_executable(tests ${SOURCES})
target_link_libraries(tests PRIVATE testdrive)

