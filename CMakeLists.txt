cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME EngineLauncher)

project(${PROJECT_NAME}
	VERSION 1.0.0
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

include(FetchContent)

FetchContent_Declare(
    CLI
    GIT_REPOSITORY "https://github.com/idDeqpt/CommandLineInterface.git"
    GIT_TAG 494ee925bbb685bef1d92beb8a97c5797a7b048d
)
FetchContent_Declare(
    JSTypes
    GIT_REPOSITORY "https://github.com/idDeqpt/JSTypes.git"
    GIT_TAG 8974efaaeab915d089ab41e2038100e9bd10fa87
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
FetchContent_MakeAvailable(CLI)
FetchContent_MakeAvailable(JSTypes)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE CLI)
target_link_libraries(${PROJECT_NAME} PRIVATE JSTypes)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist/$<CONFIG>")

file(GLOB_RECURSE EXPORT_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/export/*")

set(STAGE_STAMP "${CMAKE_BINARY_DIR}/stage.stamp")

add_custom_command(
	OUTPUT "${STAGE_STAMP}"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_DIR}/export"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${PROJECT_NAME}>
			"${DIST_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/export"
			"${DIST_DIR}/export"
	COMMAND ${CMAKE_COMMAND} -E touch "${STAGE_STAMP}"
	DEPENDS ${PROJECT_NAME} ${EXPORT_FILES}
	COMMENT "Staging into ${DIST_DIR}"
)

add_custom_target(stage ALL DEPENDS "${STAGE_STAMP}")