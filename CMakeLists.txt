cmake_minimum_required(VERSION 3.26)
project(HuHu)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(thirdparty)
add_subdirectory(src)

add_executable(HuHuApplication main.cpp)
target_link_libraries(HuHuApplication PRIVATE Rendering)
