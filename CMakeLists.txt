cmake_minimum_required(VERSION 4.1)
project(picklemul)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.4
)
FetchContent_MakeAvailable(SDL3)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

add_library(chip8_bridge SHARED
        src/mobile/chip8_bridge.cpp
        src/core/chip8.h
)
target_link_libraries(chip8_bridge PRIVATE ${JNI_LIBRARIES})

add_executable(picklemul
        src/desktop/main.cpp
        src/core/chip8.h
)
target_link_libraries(picklemul PRIVATE SDL3::SDL3-shared)

add_custom_command(TARGET picklemul POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:picklemul>
)