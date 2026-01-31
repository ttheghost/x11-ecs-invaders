cmake_minimum_required(VERSION 3.13)
project(space_invaders)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(X11 REQUIRED)

add_executable(space_invaders
        src/main.c
        src/x11_wrapper.c
        src/ecs.c
        src/factory.c
)
target_include_directories(space_invaders PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(space_invaders PRIVATE X11::X11)
