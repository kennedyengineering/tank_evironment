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


# Python Bindings
pybind11_add_module(tank_game src/binding.cpp src/engine.cpp)
target_include_directories(tank_game PRIVATE include)
target_link_libraries(tank_game PRIVATE box2d)
