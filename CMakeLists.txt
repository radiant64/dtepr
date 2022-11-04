cmake_minimum_required(VERSION 3.11)
project(dtepr)
set(CMAKE_C_STANDARD 17)

file(GLOB SOURCES src/*.c src/*.h)

add_executable(dtepr ${SOURCES})
if(${BUILD_TESTS})
    add_subdirectory(tests)
endif()

