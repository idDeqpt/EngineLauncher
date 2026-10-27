cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME AppName)

project(${PROJECT_NAME}
	VERSION 1.0.0
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_BUILD_RPATH "$ORIGIN/lib")

find_package(Engine REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Engine)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)