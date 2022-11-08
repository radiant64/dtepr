add_subdirectory(lib)

function(dtepr_add_test NAME TEST_SOURCE MODULE_SOURCE)
    add_executable(${NAME} src/${TEST_SOURCE} ../src/${MODULE_SOURCE})
    target_include_directories(${NAME} PRIVATE ../src)
    target_link_libraries(${NAME} PRIVATE testdrive)
endfunction()

set(INT_MODULE_SOURCES
    directives.c
    executor.c
    processor.c
)

dtepr_add_test(test_directives "test_directives.c" "directives.c")
dtepr_add_test(test_processor "test_processor.c" "processor.c")

add_executable(integration_tests src/integration_tests.c)
target_include_directories(integration_tests PRIVATE ../src)
target_link_libraries(integration_tests PRIVATE dtepr-lib testdrive)

