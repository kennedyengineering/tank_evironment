cmake_minimum_required(VERSION 3.22)

project(simple_simulator
        VERSION 0.1
        DESCRIPTION "A simple simulator"
        LANGUAGES C
        )

include(FetchContent)


# Box2D
FetchContent_Declare(
    box2d
    GIT_REPOSITORY  https://github.com/erincatto/box2d.git
    GIT_TAG         8e7a17c5c942dbf2fdea8b9d09983410dcc0429d    # release v3.0.0
)
FetchContent_MakeAvailable(box2d)


add_executable(test src/test.c)

set(LIBS box2d m)
target_link_libraries(test ${LIBS})


# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         7b6aead9fb88b3623e3b3725ebb42670cbe4c579    # release v3.4
)

set(GLFW_BUILD_WAYLAND 0)
set(GLFW_BUILD_DOCS 0)

FetchContent_MakeAvailable(glfw)

# (required for rendering)
find_package(OpenGL REQUIRED)


add_executable(test2 src/test_GLFW3.c)

set(LIBS2 glfw OpenGL::GL)
target_link_libraries(test2 ${LIBS2})
