cmake_minimum_required(VERSION 3.14)
project(bb)
if(POLICY CMP0135)
cmake_policy(SET CMP0135 NEW)
endif()
find_package(raylib 5.0 QUIET)
include(FetchContent)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_executable(main "main.c")
target_link_libraries(main
    raylib
)
target_include_directories(main PUBLIC "${raylib_SOURCE_DIR}/src")
