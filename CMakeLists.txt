cmake_minimum_required(VERSION 4.2.3)

project(Bloodroot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

add_executable(Bloodroot src/Main.cpp)

target_link_libraries(Bloodroot PRIVATE glfw)
target_link_libraries(Bloodroot PRIVATE Vulkan::Vulkan)
