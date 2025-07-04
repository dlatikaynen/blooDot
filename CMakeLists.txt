cmake_minimum_required(VERSION 3.31)
project(blooDot VERSION 4.0.0.0)

set(CMAKE_CXX_STANDARD 20)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(SDL)

set(SDLTTF_VENDORED ON)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)

set(SDLIMAGE_VENDORED ON)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

#set(SDLMIXER_VENDORED ON)
#add_subdirectory(SDL_mixer EXCLUDE_FROM_ALL)

set(BROTLI_BUNDLED_MODE ON)
set(BROTLI_BUILD_TOOLS OFF)
add_subdirectory(brotli)

add_executable(blooDot main.cpp
        xassy/enxassy.cpp
        xassy/enxassy.h
        xassy/dexassy.cpp
        xassy/dexassy.h
        src/util/bytefmt.h)

set_target_properties(blooDot PROPERTIES LINK_FLAGS "-static")

# all SDL3 components that we use
target_link_libraries(blooDot PUBLIC SDL3-static)
target_link_libraries(blooDot PUBLIC SDL3_ttf-static)
target_link_libraries(blooDot PUBLIC SDL3_image-static)
#target_link_libraries(blooDot PUBLIC SDL3_mixer-static)

# for xassy
target_link_libraries(blooDot PUBLIC brotlicommon)
target_link_libraries(blooDot PUBLIC brotlienc)
target_link_libraries(blooDot PUBLIC brotlidec)
include_directories(./brotli/c/include)
