set( SOURCE_FILES
	CMakeLists.txt
    anorm_dots.h
    anorms.h
    beamdef.h
    bothdefs.h
    bspfile.h
    buildnum.c
    cdll_exp.c
    cdll_int.c
    cdll_int.h
    chase.c
    cl_cam.c
    CL_DEMO.c
    cl_demo.h
    cl_draw.c
    cl_draw.h
    cl_ents.c
    client.h
    cl_input.c
    CL_MAIN.c
    CL_PARSE.c
    cl_pred.c
    cl_servercache.h
    CL_TENT.c
    cl_tent.h
    cmd.c
    cmd.h
    cmodel.c
    cmodel.h
    color.h
    common.c
    common.h
    CONSOLE.c
    CONSOLE.H
    const.h
    crc.c
    crc.h
    cshift.h
    customentity.h
    custom.h
    cvar.c
    cvardef.h
    cvar.h
    decal.h
    draw.h
    eiface.h
    GL_DRAW.c
    glHud.c
    gl_hw.h
    GL_MESH.c
    GL_MODEL.c
    GL_MODEL.H
    GLQUAKE.H
    gl_refrag.c
    gl_rlight.c
    GL_RMAIN.c
    GL_RMISC.c
    GL_RSURF.c
    gl_screen.c
    GL_VIDNT_LINUX.c
    GL_WARP.c
    gl_warp_sin.h
    gl_water.h
    hashpak.c
    hashpak.h
    host.c
    host_cmd.c
    host_cmd.h
    HUD.c
    hud_handlers.h
    input.h
    in_linux.c
    KEYS.c
    keys.h
    l_studio.c
    mathlib.c
    mathlib.h
    modelgen.h
    net_chan.c
    net.h
    net_ws.c
    pmove.c
    pmove.h
    pmovetst.c
    pr_cmds.c
    pr_cmds.h
    pr_dlls.h
    pr_edict.c
    pr_edict.h
    profile.h
    progdefs.h
    Progs.h
    protocol.h
    qgl.c
    qgl.h
    quakedef.h
    render.h
    r_local.h
    R_PART.c
    r_shared.h
    r_studio.c
    r_studio.h
    r_trans.c
    r_triangle.c
    r_triangle.h
    save.h
    sbar.h
    server.h
    shake.h
    SND_DMA.c
    SND_MEM.c
    SND_MIX.c
    snd_sdl.c
    sound.h
    spritegn.h
    studio.h
    SV_MAIN.c
    sv_move.c
    sv_phys.c
    sv_proto.h
    sv_upld.c
    sv_user.c
    sys.h
    sys.c
    sys_linux.c
    textures.c
    textures.h
    tmessage.c
    tmessage.h
    vid.h
    VIEW.c
    view.h
    vmodes.h
    wad.c
    wad.h
    world.c
    world.h
    wrect.h
    zone.c
    zone.h
)

source_group( TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES} )

add_library( hw SHARED ${SOURCE_FILES} )

target_include_directories( hw PRIVATE
	.
	${GAME_COMMON_INCLUDE_PATHS}
    ${CMAKE_SOURCE_DIR}/external/SDL2
)

message("GAME_COMMON_DEFINES=${GAME_COMMON_DEFINES}")

target_compile_definitions( hw PRIVATE
	${GAME_COMMON_DEFINES}
    GLQUAKE
    _USE_32BIT_TIME_T
)

# Dependencies
if( WIN32 )
	#set( ENGINE_PLAT_LIBS
	#	${CMAKE_SOURCE_DIR}/lib/public/SDL2.lib
	#)
	#add_library(SDL2 SHARED IMPORTED)
	#set_target_properties(SDL2 PROPERTIES
	#	IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/public/SDL2.lib"
	#)
else()
    find_package(SDL2 REQUIRED)
    set(ENGINE_PLAT_LIBS SDL2::SDL2)
endif()

# Link with dependencies
target_link_libraries( hw 
	${ENGINE_PLAT_LIBS}
)

# Linker flags
set( ENGINE_LINK_FLAGS "")
set( ENGINE_COMPILE_FLAGS "")

set_target_properties( hw PROPERTIES
    LANGUAGE C
    LINKER_LANGUAGE C
	COMPILE_FLAGS "${ENGINE_COMPILE_FLAGS} ${C_CXX_COMPILER_FLAGS}" 
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${ENGINE_LINK_FLAGS}"
)

# Remove "lib" prefix
set_target_properties( hw PROPERTIES PREFIX "" )