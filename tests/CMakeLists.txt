add_subdirectory(lib)

macro(dtepr_add_test NAME TEST_SOURCE MODULE_SOURCE)
    add_executable(${NAME} src/${TEST_SOURCE} ../src/${MODULE_SOURCE})
    target_include_directories(${NAME} PRIVATE ../src)
    target_link_libraries(${NAME} PRIVATE testdrive)
endmacro()

dtepr_add_test(test_directives test_directives.c directives.c)
dtepr_add_test(test_processor test_processor.c processor.c)

