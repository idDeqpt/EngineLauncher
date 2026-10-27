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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_BUILD_RPATH "$ORIGIN/lib")
set(SOURCE_FILES main.cpp)

include(FetchContent)

FetchContent_Declare(
	Engine
	GIT_REPOSITORY "https://github.com/idDeqpt/Engine.git"
	GIT_TAG dev
	GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(Engine)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "src/*")
add_library(App SHARED)
target_sources(App PRIVATE ${APP_SOURCES})
target_link_libraries(App PUBLIC Engine::Engine)
target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Engine)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist/$<CONFIG>")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${DIST_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${DIST_DIR}/res"
    # сам бинарник
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${PROJECT_NAME}>
            "${DIST_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
    # библиотеки (свои и Engine)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:App>
            "${DIST_DIR}/lib/$<TARGET_FILE_NAME:App>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Engine>
            "${DIST_DIR}/lib/$<TARGET_FILE_NAME:Engine>"
    # ресурсы (рекурсивно)
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/res"
            "${DIST_DIR}/res"
    COMMENT "Staging into ${DIST_DIR}"
)