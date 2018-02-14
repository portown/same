# CMakeLists.txt

cmake_minimum_required(VERSION 3.1)
enable_testing()

project(same)
set(serial "2.0.0")

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -D_NDEBUG")
set(CMAKE_LD_FLAGS_RELEASE "-mwindows -static-libgcc -static-libstdc++")

file(GLOB_RECURSE main_sources src/*.cxx)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic-errors -m64")

add_executable(Main ${main_sources} src/same.rc)
set_target_properties(Main PROPERTIES VERSION ${serial})
target_link_libraries(Main gdi32 shlwapi winmm)


file(GLOB_RECURSE test_sources test/*.cxx)
include_directories(src)
add_executable(UnitTest ${main_sources} ${test_sources})
target_link_libraries(UnitTest gdi32 shlwapi winmm CppUTest CppUTestExt)
add_test(
    NAME UnitTest
    COMMAND $<TARGET_FILE:UnitTest>)
