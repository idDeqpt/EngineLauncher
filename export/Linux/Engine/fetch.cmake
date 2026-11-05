set(ENGINE_BRANCH "dev" CACHE STRING "Engine branch to fetch")

set(ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Engine" CACHE PATH "Directory to extract Engine sources")

set(ENGINE_REFRESH OFF CACHE BOOL "Force re-download and re-extract Engine")

set(_engine_url "https://github.com/idDeqpt/Engine/archive/refs/heads/${ENGINE_BRANCH}.tar.gz")
set(_engine_archive "${CMAKE_SOURCE_DIR}/Engine-${ENGINE_BRANCH}.tar.gz")

set(_need_download TRUE)
if(EXISTS "${ENGINE_SOURCE_DIR}/CMakeLists.txt" AND NOT ENGINE_REFRESH)
    set(_need_download FALSE)
    message(STATUS "Engine sources already present in ${ENGINE_SOURCE_DIR}")
endif()

if(_need_download)
    message(STATUS "Downloading ${_engine_url}")
    file(DOWNLOAD "${_engine_url}" "${_engine_archive}"
         SHOW_PROGRESS
         TLS_VERIFY ON
         STATUS _dl_status)
    list(GET _dl_status 0 _dl_code)
    list(GET _dl_status 1 _dl_msg)
    if(NOT _dl_code EQUAL 0)
        message(FATAL_ERROR "Failed to download Engine: ${_dl_msg}")
    endif()

    set(_tmp_dir "${ENGINE_SOURCE_DIR}.tmp")
    file(REMOVE_RECURSE "${_tmp_dir}" "${ENGINE_SOURCE_DIR}")
    file(MAKE_DIRECTORY "${_tmp_dir}")

    file(ARCHIVE_EXTRACT
         INPUT       "${_engine_archive}"
         DESTINATION "${_tmp_dir}")

    file(GLOB _extracted LIST_DIRECTORIES true "${_tmp_dir}/*")
    list(LENGTH _extracted _extracted_count)
    if(_extracted_count EQUAL 1 AND IS_DIRECTORY "${_extracted}")
        file(RENAME "${_extracted}" "${ENGINE_SOURCE_DIR}")
    else()
        file(MAKE_DIRECTORY "${ENGINE_SOURCE_DIR}")
        file(COPY "${_tmp_dir}/" DESTINATION "${ENGINE_SOURCE_DIR}")
    endif()

    file(REMOVE_RECURSE "${_tmp_dir}")
	file(REMOVE         "${_engine_archive}")
    message(STATUS "Engine sources extracted to ${ENGINE_SOURCE_DIR}")
endif()