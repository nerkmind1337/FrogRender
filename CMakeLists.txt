cmake_minimum_required(VERSION 3.27)
project(FrogRender)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# GLFW library.
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_Declare(
        GLFW
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "3.4"
        GIT_PROGRESS TRUE
        SYSTEM
)

FetchContent_MakeAvailable(GLFW)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        src/main.cpp
        src/glad.c
)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)