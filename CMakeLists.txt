cmake_minimum_required(VERSION 3.31)
project(blooDot VERSION 4.0.0.0)

set(CMAKE_CXX_STANDARD 20)
enable_language(RC) # Enable RC language for resource files

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(SDL)

set(SDLTTF_VENDORED ON)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)

set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

set(SDLMIXER_VENDORED ON)
add_subdirectory(SDL_mixer EXCLUDE_FROM_ALL)

set(BROTLI_BUNDLED_MODE ON)
set(BROTLI_BUILD_TOOLS OFF)
add_subdirectory(brotli)

add_executable(blooDot blooDot.rc
        main.cpp
        xassy/enxassy.cpp
        xassy/enxassy.h
        xassy/enxlate.h
        xassy/enxlate.cpp
        xassy/dexassy.cpp
        xassy/dexassy.h
        res/xlations.cpp
        src/util/bytefmt.h
        src/util/resutil.h
        src/util/resutil.cpp
        main.h
        src/snd/sfx.h
        src/snd/sfx.cpp
        src/ui/drawing.h
        src/ui/drawing.cpp
        src/ui/splash.h
        src/ui/splash.cpp
        src/shared-constants.h
        src/ui/menu-common.h
        src/ui/menu-common.cpp
        src/util/geom2d.h
        src/util/geom2d.cpp
        src/state/settings.h
        src/state/settings.cpp
        src/ui/scripture.h
        src/ui/scripture.cpp
        src/ui/dialog-controls.h
        src/ui/dialog-controls.cpp
        src/ui/layout-constants.h
        src/ui/menu-settings.h
        src/ui/menu-settings.cpp
        src/ui/menu-settings-about.h
        src/ui/menu-settings-about.cpp
        src/ui/menu-settings-lang.h
        src/ui/menu-settings-lang.cpp
        src/ui/menu-settings-displays.h
        src/ui/menu-settings-displays.cpp
        src/algo/boydsa.h
        src/algo/boydsa.cpp
        src/ui/gpuchan.h
        src/ui/gpuchan.cpp
)

set_target_properties(blooDot PROPERTIES LINK_FLAGS "-static")

# all SDL3 components that we use
target_link_libraries(blooDot PUBLIC SDL3-static)
target_link_libraries(blooDot PUBLIC SDL3_ttf-static)
target_link_libraries(blooDot PUBLIC SDL3_image-static)
target_link_libraries(blooDot PUBLIC SDL3_mixer-static)

# other open source components that we link to
target_include_directories(blooDot PUBLIC /en/blooDot/cairo/include)
target_link_libraries(blooDot PRIVATE /en/blooDot/cairo/lib/x64/cairo.lib)

# for xassy
target_link_libraries(blooDot PUBLIC brotlicommon)
target_link_libraries(blooDot PUBLIC brotlienc)
target_link_libraries(blooDot PUBLIC brotlidec)
target_include_directories(blooDot PRIVATE ./brotli/c/include)
