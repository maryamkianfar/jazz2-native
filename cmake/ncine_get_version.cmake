if(NOT NCINE_GIT_VERSION)
	return()
endif()

find_package(Git)
if(GIT_EXECUTABLE AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE GIT_FAIL
		OUTPUT_VARIABLE GIT_REV_COUNT
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE GIT_FAIL
		OUTPUT_VARIABLE GIT_SHORT_HASH
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} log -1 --format=%ad --date=short
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE GIT_FAIL
		OUTPUT_VARIABLE GIT_LAST_COMMIT_DATE
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	#execute_process(
	#	COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
	#	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	#	RESULT_VARIABLE GIT_FAIL
	#	OUTPUT_VARIABLE GIT_BRANCH_NAME
	#	ERROR_QUIET
	#	OUTPUT_STRIP_TRAILING_WHITESPACE
	#)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --exact-match HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE GIT_NO_TAG
		OUTPUT_VARIABLE GIT_TAG_NAME
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE AA_NO_LAST_TAG
		OUTPUT_VARIABLE AA_LAST_TAG_NAME
		#ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	
	if(GIT_FAIL)
		set(GIT_NO_VERSION TRUE)
	else()
		if(GIT_NO_TAG)
			message(STATUS "GIT failed, no exact tag: ${AA_LAST_TAG_NAME} | ${AA_NO_LAST_TAG}")
			if(AA_NO_LAST_TAG)
				string(REPLACE "." ";" GIT_TAG_NAME_LIST ${AA_LAST_TAG_NAME})
				list(LENGTH GIT_TAG_NAME_LIST GIT_TAG_NAME_LIST_LENGTH)
				if(GIT_TAG_NAME_LIST_LENGTH GREATER 0)
					list(GET GIT_TAG_NAME_LIST 0 NCINE_VERSION_MAJOR)
					list(GET GIT_TAG_NAME_LIST 1 NCINE_VERSION_MINOR)
					set(NCINE_VERSION_PATCH "r${GIT_REV_COUNT}-${GIT_SHORT_HASH}")
				else()
					string(REPLACE "-" ";" GIT_LAST_COMMIT_DATE_LIST ${GIT_LAST_COMMIT_DATE})
					list(GET GIT_LAST_COMMIT_DATE_LIST 0 NCINE_VERSION_MAJOR)
					list(GET GIT_LAST_COMMIT_DATE_LIST 1 NCINE_VERSION_MINOR)
					set(NCINE_VERSION_PATCH "r${GIT_REV_COUNT}-${GIT_SHORT_HASH}")
				endif()
			else()
				string(REPLACE "-" ";" GIT_LAST_COMMIT_DATE_LIST ${GIT_LAST_COMMIT_DATE})
				list(GET GIT_LAST_COMMIT_DATE_LIST 0 NCINE_VERSION_MAJOR)
				list(GET GIT_LAST_COMMIT_DATE_LIST 1 NCINE_VERSION_MINOR)
				set(NCINE_VERSION_PATCH "r${GIT_REV_COUNT}-${GIT_SHORT_HASH}")
			endif()
		else()
			string(REPLACE "." ";" GIT_TAG_NAME_LIST ${GIT_TAG_NAME})
			list(LENGTH GIT_TAG_NAME_LIST GIT_TAG_NAME_LIST_LENGTH)
			if(GIT_TAG_NAME_LIST_LENGTH GREATER 0)
				list(GET GIT_TAG_NAME_LIST 0 NCINE_VERSION_MAJOR)
				list(GET GIT_TAG_NAME_LIST 1 NCINE_VERSION_MINOR)
				set(NCINE_VERSION_PATCH 0)
				if(GIT_TAG_NAME_LIST_LENGTH GREATER 2)
					list(GET GIT_TAG_NAME_LIST 2 NCINE_VERSION_PATCH)
				endif()
			endif()
		endif()
	endif()
else()
	set(GIT_NO_VERSION TRUE)
endif()

if(GIT_NO_VERSION)
	set(GIT_NO_TAG TRUE)
	#string(TIMESTAMP NCINE_VERSION_MAJOR "%Y")
	#string(TIMESTAMP NCINE_VERSION_MINOR "%m")
	#string(TIMESTAMP NCINE_VERSION_PATCH "%d")
	message(STATUS "GIT failed, cannot get current game version")
endif()

if(NOT GIT_NO_TAG)
	set(NCINE_VERSION "${GIT_TAG_NAME}")
else()
	set(NCINE_VERSION "${NCINE_VERSION_MAJOR}.${NCINE_VERSION_MINOR}.${NCINE_VERSION_PATCH}")
endif()
message(STATUS "Game version: ${NCINE_VERSION}")

mark_as_advanced(NCINE_VERSION_MAJOR NCINE_VERSION_MINOR NCINE_VERSION_PATCH NCINE_VERSION)
