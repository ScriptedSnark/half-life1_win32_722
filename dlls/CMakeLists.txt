set( SOURCE_FILES
	CMakeLists.txt
    activity.h
    activitymap.h
    aflock.cpp
    agrunt.cpp
    airtank.cpp
    animation.cpp
    animation.h
    apache.cpp
    barnacle.cpp
    barney.cpp
    basemonster.h
    bigmomma.cpp
    bloater.cpp
    bmodels.cpp
    bullsquid.cpp
    buttons.cpp
    cbase.cpp
    cbase.h
    cdll_dll.h
    client.cpp
    client.h
    combat.cpp
    controller.cpp
    crossbow.cpp
    crowbar.cpp
    decals.h
    defaultai.cpp
    defaultai.h
    doors.cpp
    doors.h
    effects.cpp
    effects.h
    egon.cpp
    enginecallback.h
    explode.cpp
    explode.h
    extdll.h
    flyingmonster.cpp
    flyingmonster.h
    func_break.cpp
    func_break.h
    func_tank.cpp
    gamerules.cpp
    gamerules.h
    gargantua.cpp
    gauss.cpp
    genericmonster.cpp
    ggrenade.cpp
    globals.cpp
    glock.cpp
    gman.cpp
    h_ai.cpp
    handgrenade.cpp
    hassassin.cpp
    h_battery.cpp
    h_cine.cpp
    h_cycler.cpp
    headcrab.cpp
    healthkit.cpp
    h_ai.cpp
    h_battery.cpp
    h_cine.cpp
    h_cycler.cpp
    h_export.cpp
    hgrunt.cpp
    hornet.cpp
    hornetgun.cpp
    hornet.h
    houndeye.cpp
    ichthyosaur.cpp
    islave.cpp
    items.cpp
    items.h
    leech.cpp
    lights.cpp
    monsterevent.h
    monstermaker.cpp
    monsters.cpp
    monsters.h
    monsterstate.cpp
    mortar.cpp
    mp5.cpp
    multiplay_gamerules.cpp
    nihilanth.cpp
    nodes.cpp
    nodes.h
    osprey.cpp
    pathcorner.cpp
    plane.cpp
    plane.h
    plats.cpp
    player.cpp
    player.h
    python.cpp
    rat.cpp
    roach.cpp
    rpg.cpp
    satchel.cpp
    saverestore.h
    schedule.h
    scientist.cpp
    scripted.cpp
    scripted.h
    scriptevent.h
    shotgun.cpp
    singleplay_gamerules.cpp
    skill.cpp
    skill.h
    sound.cpp
    soundent.cpp
    soundent.h
    spectator.cpp
    spectator.h
    squadmonster.cpp
    squadmonster.h
    squeakgrenade.cpp
    subs.cpp
    talkmonster.cpp
    talkmonster.h
    tentacle.cpp
    trains.h
    triggers.cpp
    tripmine.cpp
    turret.cpp
    util.cpp
    util.h
    vector.h
    weapons.cpp
    weapons.h
    world.cpp
    xen.cpp
    zombie.cpp
)

source_group( TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES} )

add_library( hl SHARED ${SOURCE_FILES} )

target_include_directories( hl PRIVATE
	.
	${GAME_COMMON_INCLUDE_PATHS}
)

message("GAME_COMMON_DEFINES=${GAME_COMMON_DEFINES}")

target_compile_definitions( hl PRIVATE
	${GAME_COMMON_DEFINES}
    QUIVER
    VOXEL
    QUAKE2
    VALVE_DLL
)

# Dependencies
if( WIN32 )
	set( SERVER_PLAT_LIBS )
else()
	set( SERVER_PLAT_LIBS )
endif()

# Link with dependencies
target_link_libraries( hl 
	${SERVER_PLAT_LIBS}
)

# Linker flags
set( SERVER_LINK_FLAGS "")
set( SERVER_COMPILE_FLAGS "")

set_target_properties( hl PROPERTIES
	COMPILE_FLAGS "${SERVER_COMPILE_FLAGS} ${C_CXX_COMPILER_FLAGS}" 
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${SERVER_LINK_FLAGS}"
)

# Remove "lib" prefix
set_target_properties( hl PROPERTIES PREFIX "" )