cmake_minimum_required(VERSION 3.30)
project(sdl2_games)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

file(GLOB_RECURSE SRC_FILES src/*.cppm src/*.ixx src/*.cpp)
file(GLOB_RECURSE MODULE_FILES src/*.cppm src/*.ixx)

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES} ${MODULE_FILES}
        src/application/components/Components.cppm)
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES ${MODULE_FILES})

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")

# Core settings
include(FetchContent)
set(BUILD_SHARED_LIBS OFF)

# Declare dependencies

FetchContent_Declare(SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL GIT_TAG release-2.32.2)
FetchContent_Declare(SDL2_TTF GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf GIT_TAG release-2.24.0)
FetchContent_Declare(SDL2_IMAGE GIT_REPOSITORY https://github.com/libsdl-org/SDL_image GIT_TAG release-2.8.8)
FetchContent_Declare(SDL2_MIXER GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer GIT_TAG release-2.8.1)
FetchContent_Declare(IMGUI GIT_REPOSITORY https://github.com/ocornut/imgui GIT_TAG v1.91.9-docking)
set(SDL_SHARED OFF CACHE BOOL "Build shared SDL2 library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static SDL2 library" FORCE)
FetchContent_MakeAvailable(SDL2)

# Configure and build SDL2_TTF with vendored dependencies
set(SDL2TTF_SDL2_SHARED OFF CACHE BOOL "Use shared SDL2 libraries" FORCE)
set(SDL2TTF_VENDORED ON CACHE BOOL "Use vendored dependencies" FORCE)
FetchContent_MakeAvailable(SDL2_TTF)

# Configure and build SDL2_IMAGE
# Update the SDL2_IMAGE configuration section like this:
set(SDL2IMAGE_SDL2_SHARED OFF CACHE BOOL "Use shared SDL2 libraries" FORCE)
set(SDL2IMAGE_VENDORED ON CACHE BOOL "Use vendored dependencies" FORCE)
set(SDL2IMAGE_AVIF OFF CACHE BOOL "Support loading AVIF images" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "Support loading WebP images" FORCE)
FetchContent_MakeAvailable(SDL2_IMAGE)

# Configure and build SDL2_MIXER
set(SDL2MIXER_SDL2_SHARED OFF CACHE BOOL "Use shared SDL2 libraries" FORCE)
set(SDL2MIXER_VENDORED ON CACHE BOOL "Use vendored dependencies" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "Support FLAC music" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "Support MOD music" FORCE)
set(SDL2MIXER_MP3 OFF CACHE BOOL "Support MP3 music" FORCE)
set(SDL2MIXER_OPUS OFF CACHE BOOL "Support Opus music" FORCE)
FetchContent_MakeAvailable(SDL2_MIXER)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_SOURCE_DIR}/include
        ${sdl2_mixer_SOURCE_DIR}/include
)

# Configure and build IMGUI
FetchContent_MakeAvailable(IMGUI)
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${SDL2_SOURCE_DIR}/include)

target_link_libraries(imgui PUBLIC SDL2::SDL2-static)

# nlohmann json
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(your_workbench_lib
        GIT_REPOSITORY https://github.com/YiRanMushroom/Your_Workbench_Lib
        GIT_TAG master
)

FetchContent_MakeAvailable(your_workbench_lib)

# Link everything
target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2-static
        SDL2::SDL2main
        Freetype::Freetype
        SDL2_ttf
        SDL2_image
        SDL2_mixer
        imgui
        nlohmann_json::nlohmann_json
        your_workbench_lib
)