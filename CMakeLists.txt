cmake_minimum_required(VERSION 3.31)
project(blooDot)

set(CMAKE_CXX_STANDARD 20)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(SDL)

set(BROTLI_BUNDLED_MODE ON)
set(BUILD_SHARED_LIBS OFF)
set(BROTLI_BUILD_TOOLS OFF)
add_subdirectory(brotli)

add_executable(blooDot main.cpp
        xassy/enxassy.cpp
        xassy/enxassy.h
        xassy/dexassy.cpp
        xassy/dexassy.h)

set_target_properties(blooDot PROPERTIES LINK_FLAGS "-static")
target_link_libraries(blooDot PUBLIC SDL3-static)
target_link_libraries(blooDot PUBLIC brotlicommon)
target_link_libraries(blooDot PUBLIC brotlienc)
include_directories(./brotli/c/include)
