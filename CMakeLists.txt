# HLNT1 CMake build file
# By ScriptedSnark

cmake_minimum_required(VERSION 3.10)

# CMake modules
list( APPEND CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_LIST_DIR}/cmake"
)

project(HLNT1 LANGUAGES C CXX)

#-----------------------------------------------------------------
# Platform specific things
#-----------------------------------------------------------------
include(PlatformInfo)

if (COMPILER_GNU)
    set( C_CXX_COMPILER_FLAGS "${C_CXX_COMPILER_FLAGS} -O0 -ggdb -Wno-write-strings -Wno-pointer-arith -Wno-endif-labels -mfpmath=sse -msse2 " )
elseif (COMPILER_MSVC)

endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_CXX_COMPILER_FLAGS} -Wno-incompatible-pointer-types -Wno-int-conversion" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_COMPILER_FLAGS}" )

#-----------------------------------------------------------------
# Preprocessor definitions
#-----------------------------------------------------------------
set( COMMON_DEFINES "" )				# Preprocessor definitions for all targets

# Add platform defines to common defines
set( COMMON_DEFINES "${COMMON_DEFINES} ${PLATFORM_DEFINES}" )

if( COMPILER_GNU )
	
	set( COMMON_DEFINES
		${COMMON_DEFINES}
		stricmp=strcasecmp
		_stricmp=strcasecmp
		_strnicmp=strncasecmp
		_snprintf=snprintf
		_alloca=alloca
		_vsnprintf=vsnprintf
		_snwprintf=swprintf
		_unlink=unlink
		_strdup=strdup
	)
	
elseif( COMPILER_MSVC )

	# Disable "unsafe" warnings
	set( COMMON_DEFINES
		${COMMON_DEFINES}
		_CRT_SECURE_NO_WARNINGS
		_SCL_SECURE_NO_WARNINGS
	)
	
endif()


set( GAME_COMMON_SRCS

)

set( GAME_COMMON_DEFINES
	${COMMON_DEFINES}
)

set( GAME_COMMON_INCLUDE_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/cl_dll
    ${CMAKE_CURRENT_SOURCE_DIR}/dlls
	${CMAKE_CURRENT_SOURCE_DIR}/common
	${CMAKE_CURRENT_SOURCE_DIR}/engine
	${CMAKE_CURRENT_SOURCE_DIR}/external
)

#-----------------------------------------------------------------
#                               QUIVER
#-----------------------------------------------------------------

add_subdirectory(cl_dll)
add_subdirectory(dlls)
add_subdirectory(shell/launcher)
add_subdirectory(engine)