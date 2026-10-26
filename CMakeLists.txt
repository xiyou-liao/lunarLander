cmake_minimum_required(VERSION 4.0)
project(lunarLander)

set(CMAKE_CXX_STANDARD 23)

add_executable(lunarLander main.cpp)

target_include_directories(lunarLander PRIVATE
        C:/raylib/include
)

target_link_directories(lunarLander PRIVATE
        C:/raylib/lib
)

target_link_libraries(lunarLander PRIVATE
        raylib
        opengl32
        gdi32
        winmm
)