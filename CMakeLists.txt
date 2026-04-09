cmake_minimum_required(VERSION 3.23)

project(Bloodroot)

set(CMAKE_CXX_STANDARD, 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)

add_executable(Bloodroot src/Main.cpp)

target_link_libraries(Bloodroot PRIVATE glfw)