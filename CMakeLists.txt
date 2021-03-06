cmake_minimum_required(VERSION 3.22)

project(
	SlickSneaking
	VERSION 1.0.0
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(FATAL_ERROR "in-source builds are not allowed")
endif()

macro(set_from_environment VARIABLE)
	if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif()
endmacro()

option(COPY_OUTPUT "copy the output of build operations to the game directory" OFF)
option(COPY_OUTPUT_MO "copy the output of build operations to the mo2 overwrite directory" OFF)
option(SKYRIMSE "set the skyrim build version to SE, defaults to AE" OFF)

if("${COPY_OUTPUT}")
	set_from_environment(Skyrim64Path)

	if(NOT DEFINED Skyrim64Path)
		message(FATAL_ERROR "Skyrim64Path is not set")
	endif()
endif()

if("${COPY_OUTPUT_MO}")
	set_from_environment(SkyrimMoPath)

	if(NOT DEFINED SkyrimMoPath)
		message(FATAL_ERROR "SkyrimMoPath is not set")
	endif()
endif()

set_from_environment(Skyrim_CI_ENV)

if(DEFINED Skyrim_CI_ENV)
	if("${Skyrim_CI_ENV}" STREQUAL "VERSION_SE")
		set(SKYRIMSE ON)
	elseif("${Skyrim_CI_ENV}" STREQUAL "VERSION_AE")
		set(SKYRIMSE OFF)
	else()
		message(FATAL_ERROR "Skyrim_CI_ENV is not set to a valid version string")
	endif()
endif()

if("${SKYRIMSE}")
	set(CommonLibPath "external/CommonLibSSE")
	add_compile_definitions(VERSION_SE)
else()
	set(CommonLibPath "external/CommonLibAE")
	add_compile_definitions(VERSION_AE)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(src)
include(cmake/packaging.cmake)
