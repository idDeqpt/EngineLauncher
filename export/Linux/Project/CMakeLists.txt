cmake_minimum_required(VERSION 3.20)

set(PROJECT_DIR "." CACHE PATH "Directory of project folder")
set(ROOT_CLASS "Root" CACHE STRING "Root class name")

set(PROJECT_NAME App)

project(${PROJECT_NAME}
	VERSION 1.0.0
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

find_package(Engine REQUIRED)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/plugin_config.hpp
    @ONLY
)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE plugin.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Engine)
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/generated
	${PROJECT_DIR}/src
)

install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION lib)
set_target_properties(${PROJECT_NAME} PROPERTIES
		INSTALL_RPATH "$ORIGIN")
install(DIRECTORY "${PROJECT_DIR}/res" DESTINATION .)