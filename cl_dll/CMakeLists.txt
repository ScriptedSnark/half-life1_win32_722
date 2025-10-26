set( SOURCE_FILES
	CMakeLists.txt
    ammo.cpp
    ammo.h
    ammohistory.cpp
    ammohistory.h
    battery.cpp
    cdll_int.cpp
    cl_dll.h
    death.cpp
    flashlight.cpp
    geiger.cpp
    health.cpp
    health.h
    hud.cpp
    hud.h
    hud_msg.cpp
    hud_redraw.cpp
    hud_update.cpp
    message.cpp
    MOTD.cpp
    parsemsg.cpp
    parsemsg.h
    saytext.cpp
    scoreboard.cpp
    train.cpp
    util.cpp
    util.h
    util_vector.h
)

source_group( TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES} )

add_library( client SHARED ${SOURCE_FILES} )

target_include_directories( client PRIVATE
	.
	${GAME_COMMON_INCLUDE_PATHS}
)

target_compile_definitions( client PRIVATE
	${GAME_COMMON_DEFINES}
)

# Dependencies
if( WIN32 )
	set( CLIENT_PLAT_LIBS )
else()
	set( CLIENT_PLAT_LIBS )
endif()

# Link with dependencies
target_link_libraries( client 
	${CLIENT_PLAT_LIBS}
)

# Linker flags
set( CLIENT_LINK_FLAGS "")
set( CLIENT_COMPILE_FLAGS "")

set_target_properties( client PROPERTIES
	COMPILE_FLAGS "${C_CXX_COMPILER_FLAGS}"
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${CLIENT_LINK_FLAGS}"
)

# Remove "lib" prefix
set_target_properties( client PROPERTIES PREFIX "" )