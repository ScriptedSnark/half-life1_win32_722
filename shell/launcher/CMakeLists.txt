set( SOURCE_FILES
    CMakeLists.txt
    dll_state.h
    exefuncs.c
    exefuncs.h
    launcher.c
    launcher.h
    sys.c
    sys.h
	util.c
	util.h
)

source_group( TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES} )

add_executable( hl_linux ${SOURCE_FILES} )

target_include_directories( hl_linux PRIVATE
	.
	${GAME_COMMON_INCLUDE_PATHS}
	${CMAKE_SOURCE_DIR}/external/SDL2
)

target_compile_definitions( hl_linux PRIVATE
	${GAME_COMMON_DEFINES}
)

# Dependencies
if( WIN32 )
	#set( LAUNCHER_PLAT_LIBS
	#	${CMAKE_SOURCE_DIR}/lib/public/SDL2.lib
	#)
	#add_library(SDL2 SHARED IMPORTED)
	#set_target_properties(SDL2 PROPERTIES
	#	IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/public/SDL2.lib"
	#)
else()
    find_package(SDL2 REQUIRED)
    set(LAUNCHER_PLAT_LIBS SDL2::SDL2)
endif()

# Link with dependencies
target_link_libraries( hl_linux
	${LAUNCHER_PLAT_LIBS}
)

# Linker flags
set( LAUNCHER_LINK_FLAGS "")
set( LAUNCHER_COMPILE_FLAGS "-fvisibility=hidden")

set_target_properties( hl_linux PROPERTIES
	LANGUAGE C
    LINKER_LANGUAGE C
	COMPILE_FLAGS "${LAUNCHER_COMPILE_FLAGS}" 
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${LAUNCHER_LINK_FLAGS}"
)

# Remove "lib" prefix
set_target_properties( hl_linux PROPERTIES PREFIX "" )