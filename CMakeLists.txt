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


# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)


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


# Tank Game Library Tests
enable_testing()

add_executable(tank_game_lib_tests
    test/test_categories.cpp
    test/test_config.cpp
    test/test_engine.cpp
    test/test_registry.cpp
    test/test_render.cpp
)

target_link_libraries(tank_game_lib_tests PRIVATE
    tank_game_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tank_game_lib_tests)


# Python Bindings
pybind11_add_module(python_bindings
    src/binding.cpp
)

target_link_libraries(python_bindings PRIVATE
    tank_game_lib
)

# FIXME: remove hard coded path
set(Python_VENV_SITELIB ${PROJECT_SOURCE_DIR}/.venv/lib/python3.12/site-packages)
if(EXISTS ${Python_VENV_SITELIB})
    message(STATUS "Found Python Virtual Environment")
    set_target_properties(python_bindings PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${Python_VENV_SITELIB})
endif()
