cmake_minimum_required(VERSION 4.2.3)

project(Bloodroot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Bloodroot src/Main.cpp)

add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

target_link_libraries(Bloodroot PRIVATE BloodrootApp)

find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/Bin)
file(GLOB SLANG_SHADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/*.slang)

function (add_slang_shader_target TARGET)
	cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

	set(SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
	set(ENTRY_POINTS -entry vertMain -entry fragMain)

	add_custom_command(
		OUTPUT ${SHADERS_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
	)

	set (TARGET_SOURCES )

	foreach (SHADER_PATH ${SHADER_SOURCES})
		get_filename_component(SHADER_NAME ${SHADER_PATH} NAME_WE)
		
		add_custom_command(
			OUTPUT ${SHADERS_DIR}/${SHADER_NAME}.spv 
			COMMAND ${SLANGC_EXECUTABLE} ${SHADER_PATH} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${SHADER_NAME}.spv
			WORKING_DIRECTORY ${SHADERS_DIR}
			DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
			COMMENT "Compiling Slang Shaders"
			VERBATIM
		)

		list(APPEND TARGET_SOURCES ${SHADERS_DIR}/${SHADER_NAME}.spv)
	endforeach()

	add_custom_target(${TARGET} DEPENDS ${TARGET_SOURCES})
endfunction()

add_slang_shader_target(shaders SOURCES ${SLANG_SHADER_SOURCES})
add_dependencies(Bloodroot shaders)

set(TEXTURE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Textures")
set(TEXTURE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/Textures")

file(MAKE_DIRECTORY ${TEXTURE_BUILD_DIR})
file(GLOB TEXTURES "${TEXTURE_DIR}/*.png")

set(TEXTURE_SOURCES "")

foreach (TEXTURE ${TEXTURES})
	cmake_path(GET TEXTURE FILENAME FILE_NAME)

	set(OUTPUT_FILE "${TEXTURE_BUILD_DIR}/${FILE_NAME}")

	add_custom_command(
		OUTPUT ${OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -E copy "${TEXTURE}" "${OUTPUT_FILE}"
		DEPENDS ${TEXTURE}
		COMMENT "Copying ${FILE_NAME} to ${OUTPUT_FILE}"
		VERBATIM
	)

	list(APPEND TEXTURE_SOURCES ${OUTPUT_FILE})
endforeach()

add_custom_target(TEXTURE_TARGET DEPENDS ${TEXTURE_SOURCES})
add_dependencies(Bloodroot TEXTURE_TARGET)

add_subdirectory(src)
