cmake_minimum_required(VERSION 3.22)

project(simple_simulator
        VERSION 0.1
        DESCRIPTION "A simple simulator"
        LANGUAGES C
        )


include(FetchContent)
FetchContent_Declare(
    box2d
    GIT_REPOSITORY  https://github.com/erincatto/box2d.git
    GIT_TAG         8e7a17c5c942dbf2fdea8b9d09983410dcc0429d    # release v3.0.0
)
FetchContent_MakeAvailable(box2d)


add_executable(test src/test.c)

set(LIBS box2d m)
target_link_libraries(test ${LIBS})
