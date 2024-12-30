cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME sdl2_games)

set(DEFAULT_PATH_TO_VCPKG "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/")

# vcpkg
set(CMAKE_TOOLCHAIN_FILE "${DEFAULT_PATH_TO_VCPKG}vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET x64-windows-static)

project(sdl2_games)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC_FILES src/*.cppm src/*.ixx src/*.cpp
)

file(GLOB_RECURSE MODULE_FILES src/*.cppm src/*.ixx
)

file(GLOB_RECURSE CPP_FILES src/*.cpp)

file(GLOB_RECURSE PCH_FILES src/headers/*.pch)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME}
        ${SRC_FILES}
        src/application/framework/layer/workableLayer.cppm
        src/application/config/applicationConstants.cppm
        src/application/functionalLayers/TopListenerLayer.cppm
        src/application/components/Basic/IWindowSizeChangedListener.cppm
        #        src/application/utilities/font/fontHolder.cppm
#        src/application/framework/sdl2/SDL2_Utilities.cppm
#        src/application/components/IComponent.cppm
#        src/application/components/Button/Buttons.cppm
#        src/application/resource/applicationResources.cppm
#        src/application/functionalLayers/AppMenuLayer.cppm
#        src/application/functionalLayers/ApplicationBasicLayers.ixx
#        src/application/functionalLayers/selectGameLayer.cppm
#        src/application/framework/accessors/virtualMachineContextProvider.cppm
#        src/headers/AppMacros.h
)

target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
#
#target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_FILES}
##    src/headers/stdc++.h
#)

target_sources(${PROJECT_NAME}
        PUBLIC
        ${CPP_FILES}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        ${MODULE_FILES}
)

# Copy resources folder to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)

# SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2::SDL2main)

# SDL2_ttf
find_package(SDL2_ttf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

# imgui - sdl2
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)