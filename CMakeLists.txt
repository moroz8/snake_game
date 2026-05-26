cmake_minimum_required(VERSION 3.16)
project(SnakeGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(snake
    src/main.cpp
    src/snake.cpp
    src/renderer.cpp
)

target_include_directories(snake PRIVATE src)
