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

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT DEFINED CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -D_NDEBUG")
set(CMAKE_LD_FLAGS_RELEASE "-mwindows -static-libgcc -static-libstdc++")

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic-errors -m64")

file(GLOB_RECURSE main_sources src/*.cxx)
add_executable(main ${main_sources} src/same.rc)
set_target_properties(main PROPERTIES VERSION ${serial})
target_link_libraries(main gdi32 shlwapi winmm)


file(GLOB_RECURSE test_sources test/*.cxx)
include_directories(src)
add_executable(unitTest ${main_sources} ${test_sources})
target_link_libraries(unitTest gdi32 shlwapi winmm CppUTest CppUTestExt)
add_test(NAME unitTest COMMAND $<TARGET_FILE:unitTest>)


file(GLOB_RECURSE data_files data/*)
file(COPY ${data_files} DESTINATION data)


install(TARGETS main DESTINATION .)
install(FILES ${data_files} DESTINATION data)
