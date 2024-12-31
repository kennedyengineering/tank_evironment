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
    GIT_REPOSITORY  https://github.com/erincatto/box2d.git
    GIT_TAG         8e7a17c5c942dbf2fdea8b9d09983410dcc0429d    # release v3.0.0
)
FetchContent_MakeAvailable(box2d)


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


# Application
add_executable(app src/main.c src/shader.c src/render.c src/engine.c)
target_include_directories(app PRIVATE include)
target_link_libraries(app box2d m glfw glad)


# PyBind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.13.6
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()


# Python Bindings
pybind11_add_module(tank_game src/binding.cpp)
