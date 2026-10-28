set(ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/Engine" CACHE PATH "Where to clone Engine")
set(REPO_URL   "git@github.com:idDeqpt/Engine.git")
set(REPO_TAG   "dev")

if(NOT EXISTS "${ENGINE_DIR}/CMakeLists.txt")
	file(MAKE_DIRECTORY "${ENGINE_DIR}")
	execute_process(
		COMMAND git clone --depth 1 --branch ${REPO_TAG} ${REPO_URL} "${ENGINE_DIR}"
		RESULT_VARIABLE result
		COMMAND_ECHO STDOUT
	)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "git clone failed: ${result}")
	endif()
endif()