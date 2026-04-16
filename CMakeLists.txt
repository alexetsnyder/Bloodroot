cmake_minimum_required(VERSION 4.2.3)

project(Bloodroot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

add_executable(Bloodroot src/Main.cpp)

target_link_libraries(Bloodroot PRIVATE glfw)
target_link_libraries(Bloodroot PRIVATE Vulkan::Vulkan)
target_link_libraries(Bloodroot PRIVATE glm::glm)

find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin)
file(GLOB SLANG_SHADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.slang)

function (add_slang_shader_target TARGET)
	cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

	set(SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
	set(ENTRY_POINTS -entry vertMain -entry fragMain)

	add_custom_command(
		OUTPUT ${SHADERS_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
	)

	add_custom_command(
		OUTPUT ${SHADERS_DIR}/slang.spv 
		COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
		WORKING_DIRECTORY ${SHADERS_DIR}
		DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
		COMMENT "Compiling Slang Shaders"
		VERBATIM
	)

	add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
endfunction()

add_slang_shader_target(shaders SOURCES ${SLANG_SHADER_SOURCES})
add_dependencies(Bloodroot shaders)