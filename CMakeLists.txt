cmake_minimum_required(VERSION 3.12)
project(dtepr)
set(CMAKE_C_STANDARD 17)

file(GLOB SOURCES src/*.c src/*.h)
list(FILTER SOURCES EXCLUDE REGEX "src\/main\.c")

add_library(dtepr-lib ${SOURCES})
add_executable(dtepr src/main.c)
target_link_libraries(dtepr PRIVATE dtepr-lib)
if(${BUILD_TESTS} EQUAL 1)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(
    TARGETS dtepr
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
)

