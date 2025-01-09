cmake_minimum_required(VERSION 3.22)


project(tank_game
        VERSION 0.1
        DESCRIPTION "Tank Game Environment"
        LANGUAGES C CXX
        )


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(FetchContent)


# Box2D
FetchContent_Declare(
    box2d
    GIT_REPOSITORY  https://github.com/erincatto/box2d
    GIT_TAG         v3.0.0
)
FetchContent_MakeAvailable(box2d)


# Cairo
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo=1.18.0)


# PyBind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)


# Tank Game Library
add_library(tank_game_lib SHARED
    src/engine.cpp
    src/tank.cpp
    src/render.cpp
)

target_include_directories(tank_game_lib PUBLIC
    include
    ${CAIRO_INCLUDE_DIRS}
)

target_link_libraries(tank_game_lib PUBLIC
    box2d
    ${CAIRO_LIBRARIES}
)


# Python Bindings
pybind11_add_module(python_bindings src/binding.cpp)
target_link_libraries(python_bindings PRIVATE tank_game_lib)
