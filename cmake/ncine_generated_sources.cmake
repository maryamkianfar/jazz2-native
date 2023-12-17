# Has to be included after ncine_get_version.cmake

set(GENERATED_SOURCE_DIR "${CMAKE_BINARY_DIR}/Generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_SOURCE_DIR}")

# Shader strings
file(GLOB SHADER_FILES "${NCINE_SOURCE_DIR}/nCine/Shaders/*.glsl")
if(NCINE_EMBED_SHADERS)
	message(STATUS "Exporting shader files to C strings")

	set(SHADERS_H_FILE "${GENERATED_INCLUDE_DIR}/shader_strings.h")
	set(SHADERS_CPP_FILE "${GENERATED_SOURCE_DIR}/shader_strings.cpp")
	if(EXISTS ${SHADERS_H_FILE})
		file(REMOVE ${SHADERS_H_FILE})
	endif()
	if(EXISTS ${SHADERS_CPP_FILE})
		file(REMOVE ${SHADERS_CPP_FILE})
	endif()

	set(SHADER_STRUCT_NAME "ShaderStrings")
	get_filename_component(SHADERS_H_FILENAME ${SHADERS_H_FILE} NAME)
	file(APPEND ${SHADERS_H_FILE} "namespace nCine {\n\n")
	file(APPEND ${SHADERS_H_FILE} "struct ${SHADER_STRUCT_NAME}\n{\n")
	file(APPEND ${SHADERS_CPP_FILE} "#include \"${SHADERS_H_FILENAME}\"\n\n")
	file(APPEND ${SHADERS_CPP_FILE} "namespace nCine {\n\n")
	foreach(SHADER_FILE ${SHADER_FILES})
		get_filename_component(SHADER_CSTRING_NAME ${SHADER_FILE} NAME_WE)
		file(STRINGS ${SHADER_FILE} SHADER_LINES NEWLINE_CONSUME)
		file(APPEND ${SHADERS_H_FILE} "\tstatic char const * const ${SHADER_CSTRING_NAME};\n")
		file(APPEND ${SHADERS_CPP_FILE} "char const * const ${SHADER_STRUCT_NAME}::${SHADER_CSTRING_NAME} = ")
		file(APPEND ${SHADERS_CPP_FILE} "R\"(\n")
		foreach(SHADER_LINE ${SHADER_LINES})
			file(APPEND ${SHADERS_CPP_FILE} "${SHADER_LINE}")
		endforeach()
		file(APPEND ${SHADERS_CPP_FILE} ")\"")
		file(APPEND ${SHADERS_CPP_FILE} ";\n\n")
	endforeach()
	file(APPEND ${SHADERS_H_FILE} "};\n\n}\n")
	file(APPEND ${SHADERS_CPP_FILE} "}\n")

	list(APPEND GENERATED_SOURCES ${SHADERS_H_FILE})
	list(APPEND GENERATED_SOURCES ${SHADERS_CPP_FILE})
	target_compile_definitions(${NCINE_APP} PRIVATE "WITH_EMBEDDED_SHADERS")
	list(APPEND ANDROID_GENERATED_FLAGS WITH_EMBEDDED_SHADERS)

	# Don't need to add shader files to the library target if they are embedded
	set(SHADER_FILES "")
endif()

if(WIN32)
	if(EXISTS "${NCINE_SOURCE_DIR}/Icons/Main.ico")
		message(STATUS "Writing a resource file for executable icons")

		set(RESOURCE_RC_FILE "${GENERATED_SOURCE_DIR}/resource.rc")
		if(NOT DEATH_TRACE OR WINDOWS_PHONE OR WINDOWS_STORE OR NOT EXISTS "${NCINE_SOURCE_DIR}/Icons/Log.ico")
			file(WRITE ${RESOURCE_RC_FILE} "GLFW_ICON ICON \"Main.ico\"")
			file(COPY "${NCINE_SOURCE_DIR}/Icons/Main.ico" DESTINATION ${GENERATED_INCLUDE_DIR})
		else()
			file(WRITE ${RESOURCE_RC_FILE} "GLFW_ICON ICON \"Main.ico\"\nLOG_ICON ICON \"Log.ico\"")
			file(COPY "${NCINE_SOURCE_DIR}/Icons/Main.ico" DESTINATION ${GENERATED_INCLUDE_DIR})
			file(COPY "${NCINE_SOURCE_DIR}/Icons/Log.ico" DESTINATION ${GENERATED_INCLUDE_DIR})
		endif()
		list(APPEND GENERATED_SOURCES ${RESOURCE_RC_FILE})
	endif()

	message(STATUS "Writing a version info resource file")
	set(PACKAGE_VERSION_PATCH ${NCINE_VERSION_PATCH})
	if(NCINE_VERSION_FROM_GIT AND GIT_NO_TAG)
		set(PACKAGE_VERSION_PATCH "0")
	endif()
	set(PACKAGE_VERSION_REV "0")
	if(DEFINED GIT_REV_COUNT)
		set(PACKAGE_VERSION_REV ${GIT_REV_COUNT})
	endif()

	set(PACKAGE_EXECUTABLE_NAME "Jazz2")
	get_target_property(DEATH_DEBUG_POSTFIX ${NCINE_APP} DEBUG_POSTFIX)
	if(NOT DEATH_DEBUG_POSTFIX)
		set(DEATH_DEBUG_POSTFIX "d")
	endif()
	set(PACKAGE_EXTENSION ".exe")
	if(WINDOWS_PHONE OR WINDOWS_STORE)
		set(PACKAGE_EXTENSION ".msixbundle")
	endif()

	set(VERSION_RC_FILE "${GENERATED_SOURCE_DIR}/Version.rc")
	configure_file("${NCINE_SOURCE_DIR}/Resources.rc.in" ${VERSION_RC_FILE} @ONLY)
	list(APPEND GENERATED_SOURCES ${VERSION_RC_FILE})

	list(APPEND GENERATED_SOURCES "${NCINE_SOURCE_DIR}/App.manifest")
endif()