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

add_executable(picklemul main.cpp
        chip8.h)

target_link_libraries(picklemul PRIVATE SDL3::SDL3-shared)
