cmake_minimum_required(VERSION 3.22)


project(simple_simulator
        VERSION 0.1
        DESCRIPTION "A simple simulator"
        LANGUAGES C CXX
        )

include(FetchContent)


# Box2D
FetchContent_Declare(
    box2d
    GIT_REPOSITORY  https://github.com/erincatto/box2d
    GIT_TAG         v3.0.0
)
FetchContent_MakeAvailable(box2d)


# PyBind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)


# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         7b6aead9fb88b3623e3b3725ebb42670cbe4c579    # release v3.4
)

set(GLFW_BUILD_WAYLAND 0)
set(GLFW_BUILD_DOCS 0)

FetchContent_MakeAvailable(glfw)


# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG        v2.0.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.5)
endif()


# Python Bindings
pybind11_add_module(tank_game src/binding.cpp src/engine.c)
target_include_directories(tank_game PRIVATE include)
target_link_libraries(tank_game PRIVATE box2d m glfw glad)
