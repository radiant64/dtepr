add_subdirectory(lib)

file(GLOB TEST_SOURCES src/*.c src/*.h)
set(MODULE_SOURCES ../src/processor.c)

add_executable(tests ${TEST_SOURCES} ${MODULE_SOURCES})
target_include_directories(tests PRIVATE ../src)
target_link_libraries(tests PRIVATE testdrive)

