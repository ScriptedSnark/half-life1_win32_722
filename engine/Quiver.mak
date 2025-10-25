# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Quiver - Win32 GL Debug
!MESSAGE No configuration specified.  Defaulting to Quiver - Win32 GL Debug.
!ENDIF 

!IF "$(CFG)" != "Quiver - Win32 Release" && "$(CFG)" != "Quiver - Win32 Debug"\
 && "$(CFG)" != "Quiver - Win32 GL Debug" && "$(CFG)" !=\
 "Quiver - Win32 GL Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quiver.mak" CFG="Quiver - Win32 GL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quiver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quiver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quiver - Win32 GL Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quiver - Win32 GL Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Quiver - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SoftRelease"
# PROP Intermediate_Dir "SoftRelease"
# PROP Target_Dir ""
OUTDIR=.\SoftRelease
INTDIR=.\SoftRelease

ALL : "$(OUTDIR)\sw.dll"

CLEAN : 
	-@erase "$(INTDIR)\a3dwrapper.obj"
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\CL_DEMO.OBJ"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\CL_MAIN.OBJ"
	-@erase "$(INTDIR)\CL_PARSE.OBJ"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\CL_TENT.OBJ"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\CONSOLE.OBJ"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\d_draw.obj"
	-@erase "$(INTDIR)\d_draw16.obj"
	-@erase "$(INTDIR)\D_EDGE.OBJ"
	-@erase "$(INTDIR)\D_FILL.OBJ"
	-@erase "$(INTDIR)\D_INIT.OBJ"
	-@erase "$(INTDIR)\D_MODECH.OBJ"
	-@erase "$(INTDIR)\D_PART.OBJ"
	-@erase "$(INTDIR)\d_parta.obj"
	-@erase "$(INTDIR)\d_polysa.obj"
	-@erase "$(INTDIR)\D_POLYSE.OBJ"
	-@erase "$(INTDIR)\D_SCAN.OBJ"
	-@erase "$(INTDIR)\d_spr8.obj"
	-@erase "$(INTDIR)\D_SPRITE.OBJ"
	-@erase "$(INTDIR)\D_SURF.OBJ"
	-@erase "$(INTDIR)\D_VARS.OBJ"
	-@erase "$(INTDIR)\d_varsa.obj"
	-@erase "$(INTDIR)\D_ZPOINT.OBJ"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\ha3dg.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\KEYS.OBJ"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\model.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\NONINTEL.OBJ"
	-@erase "$(INTDIR)\pe_win32.obj"
	-@erase "$(INTDIR)\physics.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\R_ACLIP.OBJ"
	-@erase "$(INTDIR)\r_aclipa.obj"
	-@erase "$(INTDIR)\r_alias.obj"
	-@erase "$(INTDIR)\r_aliasa.obj"
	-@erase "$(INTDIR)\R_BSP.OBJ"
	-@erase "$(INTDIR)\R_DRAW.OBJ"
	-@erase "$(INTDIR)\r_drawa.obj"
	-@erase "$(INTDIR)\R_EDGE.OBJ"
	-@erase "$(INTDIR)\r_edgea.obj"
	-@erase "$(INTDIR)\R_EFRAG.OBJ"
	-@erase "$(INTDIR)\R_LIGHT.OBJ"
	-@erase "$(INTDIR)\R_MAIN.OBJ"
	-@erase "$(INTDIR)\R_MISC.OBJ"
	-@erase "$(INTDIR)\R_PART.OBJ"
	-@erase "$(INTDIR)\R_SKY.OBJ"
	-@erase "$(INTDIR)\R_SPRITE.OBJ"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\R_SURF.OBJ"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\R_VARS.OBJ"
	-@erase "$(INTDIR)\r_varsa.obj"
	-@erase "$(INTDIR)\SCREEN.OBJ"
	-@erase "$(INTDIR)\SND_DMA.OBJ"
	-@erase "$(INTDIR)\SND_MEM.OBJ"
	-@erase "$(INTDIR)\SND_MIX.OBJ"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\SND_WIN.OBJ"
	-@erase "$(INTDIR)\surf16.obj"
	-@erase "$(INTDIR)\surf8.obj"
	-@erase "$(INTDIR)\SV_MAIN.OBJ"
	-@erase "$(INTDIR)\sv_move.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\VID_WIN.OBJ"
	-@erase "$(INTDIR)\VIEW.OBJ"
	-@erase "$(INTDIR)\wad.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\worlda.obj"
	-@erase "$(INTDIR)\zone.obj"
	-@erase "$(OUTDIR)\sw.dll"
	-@erase "$(OUTDIR)\sw.exp"
	-@erase "$(OUTDIR)\sw.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\SoftRelease/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Quiver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"SoftRelease/sw.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/sw.pdb" /machine:I386 /out:"$(OUTDIR)/sw.dll"\
 /implib:"$(OUTDIR)/sw.lib" 
LINK32_OBJS= \
	"$(INTDIR)\a3dwrapper.obj" \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\CL_DEMO.OBJ" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\CL_MAIN.OBJ" \
	"$(INTDIR)\CL_PARSE.OBJ" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\CL_TENT.OBJ" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\CONSOLE.OBJ" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\d_draw.obj" \
	"$(INTDIR)\d_draw16.obj" \
	"$(INTDIR)\D_EDGE.OBJ" \
	"$(INTDIR)\D_FILL.OBJ" \
	"$(INTDIR)\D_INIT.OBJ" \
	"$(INTDIR)\D_MODECH.OBJ" \
	"$(INTDIR)\D_PART.OBJ" \
	"$(INTDIR)\d_parta.obj" \
	"$(INTDIR)\d_polysa.obj" \
	"$(INTDIR)\D_POLYSE.OBJ" \
	"$(INTDIR)\D_SCAN.OBJ" \
	"$(INTDIR)\d_spr8.obj" \
	"$(INTDIR)\D_SPRITE.OBJ" \
	"$(INTDIR)\D_SURF.OBJ" \
	"$(INTDIR)\D_VARS.OBJ" \
	"$(INTDIR)\d_varsa.obj" \
	"$(INTDIR)\D_ZPOINT.OBJ" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\ha3dg.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\KEYS.OBJ" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\model.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\NONINTEL.OBJ" \
	"$(INTDIR)\pe_win32.obj" \
	"$(INTDIR)\physics.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\R_ACLIP.OBJ" \
	"$(INTDIR)\r_aclipa.obj" \
	"$(INTDIR)\r_alias.obj" \
	"$(INTDIR)\r_aliasa.obj" \
	"$(INTDIR)\R_BSP.OBJ" \
	"$(INTDIR)\R_DRAW.OBJ" \
	"$(INTDIR)\r_drawa.obj" \
	"$(INTDIR)\R_EDGE.OBJ" \
	"$(INTDIR)\r_edgea.obj" \
	"$(INTDIR)\R_EFRAG.OBJ" \
	"$(INTDIR)\R_LIGHT.OBJ" \
	"$(INTDIR)\R_MAIN.OBJ" \
	"$(INTDIR)\R_MISC.OBJ" \
	"$(INTDIR)\R_PART.OBJ" \
	"$(INTDIR)\R_SKY.OBJ" \
	"$(INTDIR)\R_SPRITE.OBJ" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\R_SURF.OBJ" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\R_VARS.OBJ" \
	"$(INTDIR)\r_varsa.obj" \
	"$(INTDIR)\SCREEN.OBJ" \
	"$(INTDIR)\SND_DMA.OBJ" \
	"$(INTDIR)\SND_MEM.OBJ" \
	"$(INTDIR)\SND_MIX.OBJ" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\SND_WIN.OBJ" \
	"$(INTDIR)\surf16.obj" \
	"$(INTDIR)\surf8.obj" \
	"$(INTDIR)\SV_MAIN.OBJ" \
	"$(INTDIR)\sv_move.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\VID_WIN.OBJ" \
	"$(INTDIR)\VIEW.OBJ" \
	"$(INTDIR)\wad.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\worlda.obj" \
	"$(INTDIR)\zone.obj"

"$(OUTDIR)\sw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SoftDebug"
# PROP Intermediate_Dir "SoftDebug"
# PROP Target_Dir ""
OUTDIR=.\SoftDebug
INTDIR=.\SoftDebug

ALL : "$(OUTDIR)\sw.dll"

CLEAN : 
	-@erase "$(INTDIR)\a3dwrapper.obj"
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\CL_DEMO.OBJ"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\CL_MAIN.OBJ"
	-@erase "$(INTDIR)\CL_PARSE.OBJ"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\CL_TENT.OBJ"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\CONSOLE.OBJ"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\d_draw.obj"
	-@erase "$(INTDIR)\d_draw16.obj"
	-@erase "$(INTDIR)\D_EDGE.OBJ"
	-@erase "$(INTDIR)\D_FILL.OBJ"
	-@erase "$(INTDIR)\D_INIT.OBJ"
	-@erase "$(INTDIR)\D_MODECH.OBJ"
	-@erase "$(INTDIR)\D_PART.OBJ"
	-@erase "$(INTDIR)\d_parta.obj"
	-@erase "$(INTDIR)\d_polysa.obj"
	-@erase "$(INTDIR)\D_POLYSE.OBJ"
	-@erase "$(INTDIR)\D_SCAN.OBJ"
	-@erase "$(INTDIR)\d_spr8.obj"
	-@erase "$(INTDIR)\D_SPRITE.OBJ"
	-@erase "$(INTDIR)\D_SURF.OBJ"
	-@erase "$(INTDIR)\D_VARS.OBJ"
	-@erase "$(INTDIR)\d_varsa.obj"
	-@erase "$(INTDIR)\D_ZPOINT.OBJ"
	-@erase "$(INTDIR)\draw.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\ha3dg.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\KEYS.OBJ"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\model.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\NONINTEL.OBJ"
	-@erase "$(INTDIR)\pe_win32.obj"
	-@erase "$(INTDIR)\physics.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\R_ACLIP.OBJ"
	-@erase "$(INTDIR)\r_aclipa.obj"
	-@erase "$(INTDIR)\r_alias.obj"
	-@erase "$(INTDIR)\r_aliasa.obj"
	-@erase "$(INTDIR)\R_BSP.OBJ"
	-@erase "$(INTDIR)\R_DRAW.OBJ"
	-@erase "$(INTDIR)\r_drawa.obj"
	-@erase "$(INTDIR)\R_EDGE.OBJ"
	-@erase "$(INTDIR)\r_edgea.obj"
	-@erase "$(INTDIR)\R_EFRAG.OBJ"
	-@erase "$(INTDIR)\R_LIGHT.OBJ"
	-@erase "$(INTDIR)\R_MAIN.OBJ"
	-@erase "$(INTDIR)\R_MISC.OBJ"
	-@erase "$(INTDIR)\R_PART.OBJ"
	-@erase "$(INTDIR)\R_SKY.OBJ"
	-@erase "$(INTDIR)\R_SPRITE.OBJ"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\R_SURF.OBJ"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\R_VARS.OBJ"
	-@erase "$(INTDIR)\r_varsa.obj"
	-@erase "$(INTDIR)\SCREEN.OBJ"
	-@erase "$(INTDIR)\SND_DMA.OBJ"
	-@erase "$(INTDIR)\SND_MEM.OBJ"
	-@erase "$(INTDIR)\SND_MIX.OBJ"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\SND_WIN.OBJ"
	-@erase "$(INTDIR)\surf16.obj"
	-@erase "$(INTDIR)\surf8.obj"
	-@erase "$(INTDIR)\SV_MAIN.OBJ"
	-@erase "$(INTDIR)\sv_move.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\VID_WIN.OBJ"
	-@erase "$(INTDIR)\VIEW.OBJ"
	-@erase "$(INTDIR)\wad.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\worlda.obj"
	-@erase "$(INTDIR)\zone.obj"
	-@erase "$(OUTDIR)\sw.dll"
	-@erase "$(OUTDIR)\sw.exp"
	-@erase "$(OUTDIR)\sw.ilk"
	-@erase "$(OUTDIR)\sw.lib"
	-@erase "$(OUTDIR)\sw.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\dx6sdk\include"\
 /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\SoftDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Quiver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"SoftDebug/sw.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/sw.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/sw.dll" /implib:"$(OUTDIR)/sw.lib" 
LINK32_OBJS= \
	"$(INTDIR)\a3dwrapper.obj" \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\CL_DEMO.OBJ" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\CL_MAIN.OBJ" \
	"$(INTDIR)\CL_PARSE.OBJ" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\CL_TENT.OBJ" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\CONSOLE.OBJ" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\d_draw.obj" \
	"$(INTDIR)\d_draw16.obj" \
	"$(INTDIR)\D_EDGE.OBJ" \
	"$(INTDIR)\D_FILL.OBJ" \
	"$(INTDIR)\D_INIT.OBJ" \
	"$(INTDIR)\D_MODECH.OBJ" \
	"$(INTDIR)\D_PART.OBJ" \
	"$(INTDIR)\d_parta.obj" \
	"$(INTDIR)\d_polysa.obj" \
	"$(INTDIR)\D_POLYSE.OBJ" \
	"$(INTDIR)\D_SCAN.OBJ" \
	"$(INTDIR)\d_spr8.obj" \
	"$(INTDIR)\D_SPRITE.OBJ" \
	"$(INTDIR)\D_SURF.OBJ" \
	"$(INTDIR)\D_VARS.OBJ" \
	"$(INTDIR)\d_varsa.obj" \
	"$(INTDIR)\D_ZPOINT.OBJ" \
	"$(INTDIR)\draw.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\ha3dg.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\KEYS.OBJ" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\model.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\NONINTEL.OBJ" \
	"$(INTDIR)\pe_win32.obj" \
	"$(INTDIR)\physics.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\R_ACLIP.OBJ" \
	"$(INTDIR)\r_aclipa.obj" \
	"$(INTDIR)\r_alias.obj" \
	"$(INTDIR)\r_aliasa.obj" \
	"$(INTDIR)\R_BSP.OBJ" \
	"$(INTDIR)\R_DRAW.OBJ" \
	"$(INTDIR)\r_drawa.obj" \
	"$(INTDIR)\R_EDGE.OBJ" \
	"$(INTDIR)\r_edgea.obj" \
	"$(INTDIR)\R_EFRAG.OBJ" \
	"$(INTDIR)\R_LIGHT.OBJ" \
	"$(INTDIR)\R_MAIN.OBJ" \
	"$(INTDIR)\R_MISC.OBJ" \
	"$(INTDIR)\R_PART.OBJ" \
	"$(INTDIR)\R_SKY.OBJ" \
	"$(INTDIR)\R_SPRITE.OBJ" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\R_SURF.OBJ" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\R_VARS.OBJ" \
	"$(INTDIR)\r_varsa.obj" \
	"$(INTDIR)\SCREEN.OBJ" \
	"$(INTDIR)\SND_DMA.OBJ" \
	"$(INTDIR)\SND_MEM.OBJ" \
	"$(INTDIR)\SND_MIX.OBJ" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\SND_WIN.OBJ" \
	"$(INTDIR)\surf16.obj" \
	"$(INTDIR)\surf8.obj" \
	"$(INTDIR)\SV_MAIN.OBJ" \
	"$(INTDIR)\sv_move.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\VID_WIN.OBJ" \
	"$(INTDIR)\VIEW.OBJ" \
	"$(INTDIR)\wad.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\worlda.obj" \
	"$(INTDIR)\zone.obj"

"$(OUTDIR)\sw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Quiver__"
# PROP BASE Intermediate_Dir "Quiver__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GLDebug"
# PROP Intermediate_Dir "GLDebug"
# PROP Target_Dir ""
OUTDIR=.\GLDebug
INTDIR=.\GLDebug

ALL : "$(OUTDIR)\hw.dll"

CLEAN : 
	-@erase "$(INTDIR)\a3dwrapper.obj"
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\CL_DEMO.OBJ"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\CL_MAIN.OBJ"
	-@erase "$(INTDIR)\CL_PARSE.OBJ"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\CL_TENT.OBJ"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\CONSOLE.OBJ"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\GL_DRAW.OBJ"
	-@erase "$(INTDIR)\GL_MESH.OBJ"
	-@erase "$(INTDIR)\GL_MODEL.OBJ"
	-@erase "$(INTDIR)\gl_refrag.obj"
	-@erase "$(INTDIR)\gl_rlight.obj"
	-@erase "$(INTDIR)\GL_RMAIN.OBJ"
	-@erase "$(INTDIR)\GL_RMISC.OBJ"
	-@erase "$(INTDIR)\GL_RSURF.OBJ"
	-@erase "$(INTDIR)\gl_screen.obj"
	-@erase "$(INTDIR)\GL_VIDNT.OBJ"
	-@erase "$(INTDIR)\GL_WARP.OBJ"
	-@erase "$(INTDIR)\glHud.obj"
	-@erase "$(INTDIR)\glide.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\ha3dg.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\KEYS.OBJ"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\opengl32.obj"
	-@erase "$(INTDIR)\pe_win32.obj"
	-@erase "$(INTDIR)\physics.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\R_PART.OBJ"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\SND_DMA.OBJ"
	-@erase "$(INTDIR)\SND_MEM.OBJ"
	-@erase "$(INTDIR)\SND_MIX.OBJ"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\SND_WIN.OBJ"
	-@erase "$(INTDIR)\SV_MAIN.OBJ"
	-@erase "$(INTDIR)\sv_move.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\VIEW.OBJ"
	-@erase "$(INTDIR)\wad.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\worlda.obj"
	-@erase "$(INTDIR)\zone.obj"
	-@erase "$(OUTDIR)\hw.dll"
	-@erase "$(OUTDIR)\hw.exp"
	-@erase "$(OUTDIR)\hw.ilk"
	-@erase "$(OUTDIR)\hw.lib"
	-@erase "$(OUTDIR)\hw.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\dx6sdk\include"\
 /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\GLDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Quiver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"GLDebug/hw.dll"
# ADD LINK32 ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"GLDebug/hw.dll" /libpath:"..\dx6sdk\lib"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/hw.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/hw.dll" /implib:"$(OUTDIR)/hw.lib"\
 /libpath:"..\dx6sdk\lib" 
LINK32_OBJS= \
	"$(INTDIR)\a3dwrapper.obj" \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\CL_DEMO.OBJ" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\CL_MAIN.OBJ" \
	"$(INTDIR)\CL_PARSE.OBJ" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\CL_TENT.OBJ" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\CONSOLE.OBJ" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\GL_DRAW.OBJ" \
	"$(INTDIR)\GL_MESH.OBJ" \
	"$(INTDIR)\GL_MODEL.OBJ" \
	"$(INTDIR)\gl_refrag.obj" \
	"$(INTDIR)\gl_rlight.obj" \
	"$(INTDIR)\GL_RMAIN.OBJ" \
	"$(INTDIR)\GL_RMISC.OBJ" \
	"$(INTDIR)\GL_RSURF.OBJ" \
	"$(INTDIR)\gl_screen.obj" \
	"$(INTDIR)\GL_VIDNT.OBJ" \
	"$(INTDIR)\GL_WARP.OBJ" \
	"$(INTDIR)\glHud.obj" \
	"$(INTDIR)\glide.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\ha3dg.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\KEYS.OBJ" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\opengl32.obj" \
	"$(INTDIR)\pe_win32.obj" \
	"$(INTDIR)\physics.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\R_PART.OBJ" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\SND_DMA.OBJ" \
	"$(INTDIR)\SND_MEM.OBJ" \
	"$(INTDIR)\SND_MIX.OBJ" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\SND_WIN.OBJ" \
	"$(INTDIR)\SV_MAIN.OBJ" \
	"$(INTDIR)\sv_move.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\VIEW.OBJ" \
	"$(INTDIR)\wad.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\worlda.obj" \
	"$(INTDIR)\zone.obj"

"$(OUTDIR)\hw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Quiver_0"
# PROP BASE Intermediate_Dir "Quiver_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GLRelease"
# PROP Intermediate_Dir "GLRelease"
# PROP Target_Dir ""
OUTDIR=.\GLRelease
INTDIR=.\GLRelease

ALL : "$(OUTDIR)\hw.dll"

CLEAN : 
	-@erase "$(INTDIR)\a3dwrapper.obj"
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\CL_DEMO.OBJ"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\CL_MAIN.OBJ"
	-@erase "$(INTDIR)\CL_PARSE.OBJ"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\CL_TENT.OBJ"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\CONSOLE.OBJ"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\GL_DRAW.OBJ"
	-@erase "$(INTDIR)\GL_MESH.OBJ"
	-@erase "$(INTDIR)\GL_MODEL.OBJ"
	-@erase "$(INTDIR)\gl_refrag.obj"
	-@erase "$(INTDIR)\gl_rlight.obj"
	-@erase "$(INTDIR)\GL_RMAIN.OBJ"
	-@erase "$(INTDIR)\GL_RMISC.OBJ"
	-@erase "$(INTDIR)\GL_RSURF.OBJ"
	-@erase "$(INTDIR)\gl_screen.obj"
	-@erase "$(INTDIR)\GL_VIDNT.OBJ"
	-@erase "$(INTDIR)\GL_WARP.OBJ"
	-@erase "$(INTDIR)\glHud.obj"
	-@erase "$(INTDIR)\glide.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\ha3dg.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\KEYS.OBJ"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\opengl32.obj"
	-@erase "$(INTDIR)\pe_win32.obj"
	-@erase "$(INTDIR)\physics.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\R_PART.OBJ"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\SND_DMA.OBJ"
	-@erase "$(INTDIR)\SND_MEM.OBJ"
	-@erase "$(INTDIR)\SND_MIX.OBJ"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\SND_WIN.OBJ"
	-@erase "$(INTDIR)\SV_MAIN.OBJ"
	-@erase "$(INTDIR)\sv_move.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\VIEW.OBJ"
	-@erase "$(INTDIR)\wad.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\worlda.obj"
	-@erase "$(INTDIR)\zone.obj"
	-@erase "$(OUTDIR)\hw.dll"
	-@erase "$(OUTDIR)\hw.exp"
	-@erase "$(OUTDIR)\hw.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\common" /D "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\GLRelease/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Quiver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"GLRelease/hw.dll"
# ADD LINK32 ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"GLRelease/hw.dll" /libpath:"..\dx6sdk\lib"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/hw.pdb" /machine:I386\
 /out:"$(OUTDIR)/hw.dll" /implib:"$(OUTDIR)/hw.lib" /libpath:"..\dx6sdk\lib" 
LINK32_OBJS= \
	"$(INTDIR)\a3dwrapper.obj" \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\CL_DEMO.OBJ" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\CL_MAIN.OBJ" \
	"$(INTDIR)\CL_PARSE.OBJ" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\CL_TENT.OBJ" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\CONSOLE.OBJ" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\GL_DRAW.OBJ" \
	"$(INTDIR)\GL_MESH.OBJ" \
	"$(INTDIR)\GL_MODEL.OBJ" \
	"$(INTDIR)\gl_refrag.obj" \
	"$(INTDIR)\gl_rlight.obj" \
	"$(INTDIR)\GL_RMAIN.OBJ" \
	"$(INTDIR)\GL_RMISC.OBJ" \
	"$(INTDIR)\GL_RSURF.OBJ" \
	"$(INTDIR)\gl_screen.obj" \
	"$(INTDIR)\GL_VIDNT.OBJ" \
	"$(INTDIR)\GL_WARP.OBJ" \
	"$(INTDIR)\glHud.obj" \
	"$(INTDIR)\glide.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\ha3dg.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\KEYS.OBJ" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\opengl32.obj" \
	"$(INTDIR)\pe_win32.obj" \
	"$(INTDIR)\physics.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\R_PART.OBJ" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\SND_DMA.OBJ" \
	"$(INTDIR)\SND_MEM.OBJ" \
	"$(INTDIR)\SND_MIX.OBJ" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\SND_WIN.OBJ" \
	"$(INTDIR)\SV_MAIN.OBJ" \
	"$(INTDIR)\sv_move.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\VIEW.OBJ" \
	"$(INTDIR)\wad.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\worlda.obj" \
	"$(INTDIR)\zone.obj"

"$(OUTDIR)\hw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Quiver - Win32 Release"
# Name "Quiver - Win32 Debug"
# Name "Quiver - Win32 GL Debug"
# Name "Quiver - Win32 GL Release"

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\adivtab.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\anorm_dots.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\anorms.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\beamdef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bothdefs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bspfile.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\buildnum.c
DEP_CPP_BUILD=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_BUILD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\buildnum.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\buildnum.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\buildnum.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\buildnum.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_exp.c
DEP_CPP_CDLL_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CDLL_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cdll_exp.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cdll_exp.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cdll_exp.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cdll_exp.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_int.c
DEP_CPP_CDLL_I=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\cl_draw.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\hud_handlers.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\tmessage.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CDLL_I=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_I) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_I) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_I) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_I) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_int.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\chase.c
DEP_CPP_CHASE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CHASE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\chase.obj" : $(SOURCE) $(DEP_CPP_CHASE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\chase.obj" : $(SOURCE) $(DEP_CPP_CHASE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\chase.obj" : $(SOURCE) $(DEP_CPP_CHASE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\chase.obj" : $(SOURCE) $(DEP_CPP_CHASE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_cam.c
DEP_CPP_CL_CA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_CA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cl_cam.obj" : $(SOURCE) $(DEP_CPP_CL_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cl_cam.obj" : $(SOURCE) $(DEP_CPP_CL_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cl_cam.obj" : $(SOURCE) $(DEP_CPP_CL_CA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cl_cam.obj" : $(SOURCE) $(DEP_CPP_CL_CA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CL_DEMO.C
DEP_CPP_CL_DE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\shake.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\tmessage.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_DE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\CL_DEMO.OBJ" : $(SOURCE) $(DEP_CPP_CL_DE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\CL_DEMO.OBJ" : $(SOURCE) $(DEP_CPP_CL_DE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\CL_DEMO.OBJ" : $(SOURCE) $(DEP_CPP_CL_DE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\CL_DEMO.OBJ" : $(SOURCE) $(DEP_CPP_CL_DE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_demo.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_draw.c
DEP_CPP_CL_DR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_draw.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_DR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cl_draw.obj" : $(SOURCE) $(DEP_CPP_CL_DR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cl_draw.obj" : $(SOURCE) $(DEP_CPP_CL_DR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cl_draw.obj" : $(SOURCE) $(DEP_CPP_CL_DR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cl_draw.obj" : $(SOURCE) $(DEP_CPP_CL_DR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_draw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_ents.c
DEP_CPP_CL_EN=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_EN=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cl_ents.obj" : $(SOURCE) $(DEP_CPP_CL_EN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cl_ents.obj" : $(SOURCE) $(DEP_CPP_CL_EN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cl_ents.obj" : $(SOURCE) $(DEP_CPP_CL_EN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cl_ents.obj" : $(SOURCE) $(DEP_CPP_CL_EN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_input.c
DEP_CPP_CL_IN=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_IN=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cl_input.obj" : $(SOURCE) $(DEP_CPP_CL_IN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cl_input.obj" : $(SOURCE) $(DEP_CPP_CL_IN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cl_input.obj" : $(SOURCE) $(DEP_CPP_CL_IN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cl_input.obj" : $(SOURCE) $(DEP_CPP_CL_IN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CL_MAIN.C
DEP_CPP_CL_MA=\
	"..\common\clientid.h"\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\cl_servercache.h"\
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\tmessage.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_MA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\CL_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_CL_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\CL_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_CL_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\CL_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_CL_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\CL_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_CL_MA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CL_PARSE.C
DEP_CPP_CL_PA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\cl_draw.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_trans.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_PA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\CL_PARSE.OBJ" : $(SOURCE) $(DEP_CPP_CL_PA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\CL_PARSE.OBJ" : $(SOURCE) $(DEP_CPP_CL_PA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\CL_PARSE.OBJ" : $(SOURCE) $(DEP_CPP_CL_PA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\CL_PARSE.OBJ" : $(SOURCE) $(DEP_CPP_CL_PA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_pred.c
DEP_CPP_CL_PR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_PR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cl_pred.obj" : $(SOURCE) $(DEP_CPP_CL_PR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cl_pred.obj" : $(SOURCE) $(DEP_CPP_CL_PR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cl_pred.obj" : $(SOURCE) $(DEP_CPP_CL_PR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cl_pred.obj" : $(SOURCE) $(DEP_CPP_CL_PR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_servercache.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CL_TENT.C
DEP_CPP_CL_TE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	"..\common\r_efx.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CL_TE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\CL_TENT.OBJ" : $(SOURCE) $(DEP_CPP_CL_TE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\CL_TENT.OBJ" : $(SOURCE) $(DEP_CPP_CL_TE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\CL_TENT.OBJ" : $(SOURCE) $(DEP_CPP_CL_TE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\CL_TENT.OBJ" : $(SOURCE) $(DEP_CPP_CL_TE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_tent.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\client.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmd.c
DEP_CPP_CMD_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CMD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cmd.obj" : $(SOURCE) $(DEP_CPP_CMD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cmd.obj" : $(SOURCE) $(DEP_CPP_CMD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cmd.obj" : $(SOURCE) $(DEP_CPP_CMD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cmd.obj" : $(SOURCE) $(DEP_CPP_CMD_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmd.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmodel.c
DEP_CPP_CMODE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CMODE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cmodel.obj" : $(SOURCE) $(DEP_CPP_CMODE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cmodel.obj" : $(SOURCE) $(DEP_CPP_CMODE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cmodel.obj" : $(SOURCE) $(DEP_CPP_CMODE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cmodel.obj" : $(SOURCE) $(DEP_CPP_CMODE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmodel.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\color.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\common.c
DEP_CPP_COMMO=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_COMMO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\common.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\conproc.c
DEP_CPP_CONPR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\conproc.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CONPR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\conproc.obj" : $(SOURCE) $(DEP_CPP_CONPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\conproc.obj" : $(SOURCE) $(DEP_CPP_CONPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\conproc.obj" : $(SOURCE) $(DEP_CPP_CONPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\conproc.obj" : $(SOURCE) $(DEP_CPP_CONPR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\conproc.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CONSOLE.C
DEP_CPP_CONSO=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CONSO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\CONSOLE.OBJ" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\CONSOLE.OBJ" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\CONSOLE.OBJ" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\CONSOLE.OBJ" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CONSOLE.H

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\const.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crc.c
DEP_CPP_CRC_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CRC_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\crc.obj" : $(SOURCE) $(DEP_CPP_CRC_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\crc.obj" : $(SOURCE) $(DEP_CPP_CRC_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\crc.obj" : $(SOURCE) $(DEP_CPP_CRC_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\crc.obj" : $(SOURCE) $(DEP_CPP_CRC_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crc.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cshift.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\custom.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\customentity.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cvar.c
DEP_CPP_CVAR_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_CVAR_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\cvar.obj" : $(SOURCE) $(DEP_CPP_CVAR_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\cvar.obj" : $(SOURCE) $(DEP_CPP_CVAR_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\cvar.obj" : $(SOURCE) $(DEP_CPP_CVAR_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\cvar.obj" : $(SOURCE) $(DEP_CPP_CVAR_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cvar.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cvardef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_EDGE.C
DEP_CPP_D_EDG=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\water.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_EDG=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_EDGE.OBJ" : $(SOURCE) $(DEP_CPP_D_EDG) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_EDGE.OBJ" : $(SOURCE) $(DEP_CPP_D_EDG) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_FILL.C
DEP_CPP_D_FIL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_FIL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_FILL.OBJ" : $(SOURCE) $(DEP_CPP_D_FIL) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_FILL.OBJ" : $(SOURCE) $(DEP_CPP_D_FIL) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_iface.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_INIT.C
DEP_CPP_D_INI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_INI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_INIT.OBJ" : $(SOURCE) $(DEP_CPP_D_INI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_INIT.OBJ" : $(SOURCE) $(DEP_CPP_D_INI) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_local.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_MODECH.C
DEP_CPP_D_MOD=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_MOD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_MODECH.OBJ" : $(SOURCE) $(DEP_CPP_D_MOD) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_MODECH.OBJ" : $(SOURCE) $(DEP_CPP_D_MOD) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_PART.C
DEP_CPP_D_PAR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_PAR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_PART.OBJ" : $(SOURCE) $(DEP_CPP_D_PAR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_PART.OBJ" : $(SOURCE) $(DEP_CPP_D_PAR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_POLYSE.C
DEP_CPP_D_POL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\adivtab.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_POL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_POLYSE.OBJ" : $(SOURCE) $(DEP_CPP_D_POL) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_POLYSE.OBJ" : $(SOURCE) $(DEP_CPP_D_POL) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_SCAN.C
DEP_CPP_D_SCA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_SCA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_SCAN.OBJ" : $(SOURCE) $(DEP_CPP_D_SCA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_SCAN.OBJ" : $(SOURCE) $(DEP_CPP_D_SCA) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_SPRITE.C
DEP_CPP_D_SPR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_SPR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_SPRITE.OBJ" : $(SOURCE) $(DEP_CPP_D_SPR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_SPRITE.OBJ" : $(SOURCE) $(DEP_CPP_D_SPR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_SURF.C
DEP_CPP_D_SUR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\water.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_SUR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_SURF.OBJ" : $(SOURCE) $(DEP_CPP_D_SUR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_SURF.OBJ" : $(SOURCE) $(DEP_CPP_D_SUR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_VARS.C
DEP_CPP_D_VAR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_VAR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_VARS.OBJ" : $(SOURCE) $(DEP_CPP_D_VAR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_VARS.OBJ" : $(SOURCE) $(DEP_CPP_D_VAR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\D_ZPOINT.C
DEP_CPP_D_ZPO=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_D_ZPO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\D_ZPOINT.OBJ" : $(SOURCE) $(DEP_CPP_D_ZPO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\D_ZPOINT.OBJ" : $(SOURCE) $(DEP_CPP_D_ZPO) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\decal.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.c
DEP_CPP_DRAW_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_DRAW_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\eiface.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_DRAW.C
DEP_CPP_GL_DR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_DR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_DRAW.OBJ" : $(SOURCE) $(DEP_CPP_GL_DR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_DRAW.OBJ" : $(SOURCE) $(DEP_CPP_GL_DR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_hw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_MESH.C
DEP_CPP_GL_ME=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_ME=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_MESH.OBJ" : $(SOURCE) $(DEP_CPP_GL_ME) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_MESH.OBJ" : $(SOURCE) $(DEP_CPP_GL_ME) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_MODEL.C
DEP_CPP_GL_MO=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\gl_water.h"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\textures.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_MO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_MODEL.OBJ" : $(SOURCE) $(DEP_CPP_GL_MO) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_MODEL.OBJ" : $(SOURCE) $(DEP_CPP_GL_MO) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_MODEL.H

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_refrag.c
DEP_CPP_GL_RE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_RE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\gl_refrag.obj" : $(SOURCE) $(DEP_CPP_GL_RE) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\gl_refrag.obj" : $(SOURCE) $(DEP_CPP_GL_RE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_rlight.c
DEP_CPP_GL_RL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_RL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\gl_rlight.obj" : $(SOURCE) $(DEP_CPP_GL_RL) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\gl_rlight.obj" : $(SOURCE) $(DEP_CPP_GL_RL) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_RMAIN.C
DEP_CPP_GL_RM=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\anorm_dots.h"\
	".\anorms.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\shake.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_RM=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_RMAIN.OBJ" : $(SOURCE) $(DEP_CPP_GL_RM) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_RMAIN.OBJ" : $(SOURCE) $(DEP_CPP_GL_RM) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_RMISC.C
DEP_CPP_GL_RMI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_RMI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_RMISC.OBJ" : $(SOURCE) $(DEP_CPP_GL_RMI) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_RMISC.OBJ" : $(SOURCE) $(DEP_CPP_GL_RMI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_RSURF.C
DEP_CPP_GL_RS=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\gl_water.h"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_RS=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_RSURF.OBJ" : $(SOURCE) $(DEP_CPP_GL_RS) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_RSURF.OBJ" : $(SOURCE) $(DEP_CPP_GL_RS) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_screen.c
DEP_CPP_GL_SC=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_SC=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\gl_screen.obj" : $(SOURCE) $(DEP_CPP_GL_SC) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\gl_screen.obj" : $(SOURCE) $(DEP_CPP_GL_SC) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_VIDNT.C
DEP_CPP_GL_VI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\gl_hw.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_VI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_VIDNT.OBJ" : $(SOURCE) $(DEP_CPP_GL_VI) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_VIDNT.OBJ" : $(SOURCE) $(DEP_CPP_GL_VI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GL_WARP.C
DEP_CPP_GL_WA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\gl_warp_sin.h"\
	".\gl_water.h"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GL_WA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\GL_WARP.OBJ" : $(SOURCE) $(DEP_CPP_GL_WA) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\GL_WARP.OBJ" : $(SOURCE) $(DEP_CPP_GL_WA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_warp_sin.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_water.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\glHud.c
DEP_CPP_GLHUD=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GLHUD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\glHud.obj" : $(SOURCE) $(DEP_CPP_GLHUD) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\glHud.obj" : $(SOURCE) $(DEP_CPP_GLHUD) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\glide.c
DEP_CPP_GLIDE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	"..\common\winsani_in.h"\
	"..\common\winsani_out.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_GLIDE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\glide.obj" : $(SOURCE) $(DEP_CPP_GLIDE) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\glide.obj" : $(SOURCE) $(DEP_CPP_GLIDE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GLQUAKE.H

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ha3d.cpp
DEP_CPP_HA3D_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\snd_a3d.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HA3D_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\ha3d.obj" : $(SOURCE) $(DEP_CPP_HA3D_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\ha3d.obj" : $(SOURCE) $(DEP_CPP_HA3D_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\ha3d.obj" : $(SOURCE) $(DEP_CPP_HA3D_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\ha3d.obj" : $(SOURCE) $(DEP_CPP_HA3D_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ha3dg.cpp
DEP_CPP_HA3DG=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\snd_a3d.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HA3DG=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\ha3dg.obj" : $(SOURCE) $(DEP_CPP_HA3DG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\ha3dg.obj" : $(SOURCE) $(DEP_CPP_HA3DG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\ha3dg.obj" : $(SOURCE) $(DEP_CPP_HA3DG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\ha3dg.obj" : $(SOURCE) $(DEP_CPP_HA3DG) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hashpak.c
DEP_CPP_HASHP=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HASHP=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\hashpak.obj" : $(SOURCE) $(DEP_CPP_HASHP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\hashpak.obj" : $(SOURCE) $(DEP_CPP_HASHP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\hashpak.obj" : $(SOURCE) $(DEP_CPP_HASHP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\hashpak.obj" : $(SOURCE) $(DEP_CPP_HASHP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hashpak.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\host.c
DEP_CPP_HOST_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HOST_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\host.obj" : $(SOURCE) $(DEP_CPP_HOST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\host.obj" : $(SOURCE) $(DEP_CPP_HOST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\host.obj" : $(SOURCE) $(DEP_CPP_HOST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\host.obj" : $(SOURCE) $(DEP_CPP_HOST_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\host_cmd.c
DEP_CPP_HOST_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\pr_edict.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HOST_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\host_cmd.obj" : $(SOURCE) $(DEP_CPP_HOST_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\host_cmd.obj" : $(SOURCE) $(DEP_CPP_HOST_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\host_cmd.obj" : $(SOURCE) $(DEP_CPP_HOST_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\host_cmd.obj" : $(SOURCE) $(DEP_CPP_HOST_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\host_cmd.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HUD.c
DEP_CPP_HUD_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_HUD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\HUD.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\HUD.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\HUD.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\HUD.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud_handlers.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\in_win.c
DEP_CPP_IN_WI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_IN_WI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\in_win.obj" : $(SOURCE) $(DEP_CPP_IN_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\in_win.obj" : $(SOURCE) $(DEP_CPP_IN_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\in_win.obj" : $(SOURCE) $(DEP_CPP_IN_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\in_win.obj" : $(SOURCE) $(DEP_CPP_IN_WI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\input.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\KEYS.C
DEP_CPP_KEYS_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_KEYS_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\KEYS.OBJ" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\KEYS.OBJ" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\KEYS.OBJ" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\KEYS.OBJ" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keys.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\l_studio.c
DEP_CPP_L_STU=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_L_STU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\l_studio.obj" : $(SOURCE) $(DEP_CPP_L_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\l_studio.obj" : $(SOURCE) $(DEP_CPP_L_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\l_studio.obj" : $(SOURCE) $(DEP_CPP_L_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\l_studio.obj" : $(SOURCE) $(DEP_CPP_L_STU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mathlib.c
DEP_CPP_MATHL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_MATHL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\mathlib.obj" : $(SOURCE) $(DEP_CPP_MATHL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\mathlib.obj" : $(SOURCE) $(DEP_CPP_MATHL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\mathlib.obj" : $(SOURCE) $(DEP_CPP_MATHL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\mathlib.obj" : $(SOURCE) $(DEP_CPP_MATHL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mathlib.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\model.c
DEP_CPP_MODEL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\textures.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_MODEL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\model.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\model.obj" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\model.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\net.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\net_chan.c
DEP_CPP_NET_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_NET_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\net_chan.obj" : $(SOURCE) $(DEP_CPP_NET_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\net_chan.obj" : $(SOURCE) $(DEP_CPP_NET_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\net_chan.obj" : $(SOURCE) $(DEP_CPP_NET_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\net_chan.obj" : $(SOURCE) $(DEP_CPP_NET_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\net_ws.c
DEP_CPP_NET_W=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_NET_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\net_ws.obj" : $(SOURCE) $(DEP_CPP_NET_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\net_ws.obj" : $(SOURCE) $(DEP_CPP_NET_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\net_ws.obj" : $(SOURCE) $(DEP_CPP_NET_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\net_ws.obj" : $(SOURCE) $(DEP_CPP_NET_W) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NONINTEL.C
DEP_CPP_NONIN=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_NONIN=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\NONINTEL.OBJ" : $(SOURCE) $(DEP_CPP_NONIN) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\NONINTEL.OBJ" : $(SOURCE) $(DEP_CPP_NONIN) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\opengl32.cpp
DEP_CPP_OPENG=\
	"..\common\../dx6sdk/include/d3d.h"\
	"..\common\../dx6sdk/include/ddraw.h"\
	"..\common\d3d_structs.h"\
	"..\dx6sdk\include\d3dcaps.h"\
	"..\dx6sdk\include\d3dtypes.h"\
	"..\dx6sdk\include\d3dvec.inl"\
	".\opengl32.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\opengl32.obj" : $(SOURCE) $(DEP_CPP_OPENG) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\opengl32.obj" : $(SOURCE) $(DEP_CPP_OPENG) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\opengl32.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pe_win32.c
DEP_CPP_PE_WI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PE_WI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\pe_win32.obj" : $(SOURCE) $(DEP_CPP_PE_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\pe_win32.obj" : $(SOURCE) $(DEP_CPP_PE_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\pe_win32.obj" : $(SOURCE) $(DEP_CPP_PE_WI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\pe_win32.obj" : $(SOURCE) $(DEP_CPP_PE_WI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\physics.c
DEP_CPP_PHYSI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PHYSI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\physics.obj" : $(SOURCE) $(DEP_CPP_PHYSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\physics.obj" : $(SOURCE) $(DEP_CPP_PHYSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\physics.obj" : $(SOURCE) $(DEP_CPP_PHYSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\physics.obj" : $(SOURCE) $(DEP_CPP_PHYSI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pmove.c
DEP_CPP_PMOVE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sv_proto.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PMOVE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\pmove.obj" : $(SOURCE) $(DEP_CPP_PMOVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\pmove.obj" : $(SOURCE) $(DEP_CPP_PMOVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\pmove.obj" : $(SOURCE) $(DEP_CPP_PMOVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\pmove.obj" : $(SOURCE) $(DEP_CPP_PMOVE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pmove.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pmovetst.c
DEP_CPP_PMOVET=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PMOVET=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\pmovetst.obj" : $(SOURCE) $(DEP_CPP_PMOVET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\pmovetst.obj" : $(SOURCE) $(DEP_CPP_PMOVET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\pmovetst.obj" : $(SOURCE) $(DEP_CPP_PMOVET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\pmovetst.obj" : $(SOURCE) $(DEP_CPP_PMOVET) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_cmds.c
DEP_CPP_PR_CM=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sv_proto.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PR_CM=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\pr_cmds.obj" : $(SOURCE) $(DEP_CPP_PR_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\pr_cmds.obj" : $(SOURCE) $(DEP_CPP_PR_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\pr_cmds.obj" : $(SOURCE) $(DEP_CPP_PR_CM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\pr_cmds.obj" : $(SOURCE) $(DEP_CPP_PR_CM) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_cmds.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_dlls.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_edict.c
DEP_CPP_PR_ED=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\pr_edict.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_PR_ED=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\pr_edict.obj" : $(SOURCE) $(DEP_CPP_PR_ED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\pr_edict.obj" : $(SOURCE) $(DEP_CPP_PR_ED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\pr_edict.obj" : $(SOURCE) $(DEP_CPP_PR_ED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\pr_edict.obj" : $(SOURCE) $(DEP_CPP_PR_ED) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_edict.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\profile.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\progdefs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Progs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\protocol.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\qgl.c
DEP_CPP_QGL_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_QGL_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\qgl.obj" : $(SOURCE) $(DEP_CPP_QGL_C) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D\
 "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\qgl.obj" : $(SOURCE) $(DEP_CPP_QGL_C) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\qgl.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\quakedef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_ACLIP.C
DEP_CPP_R_ACL=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_ACL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_ACLIP.OBJ" : $(SOURCE) $(DEP_CPP_R_ACL) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_ACLIP.OBJ" : $(SOURCE) $(DEP_CPP_R_ACL) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_alias.c
DEP_CPP_R_ALI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\anorms.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_ALI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\r_alias.obj" : $(SOURCE) $(DEP_CPP_R_ALI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\r_alias.obj" : $(SOURCE) $(DEP_CPP_R_ALI) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_BSP.C
DEP_CPP_R_BSP=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_BSP=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_BSP.OBJ" : $(SOURCE) $(DEP_CPP_R_BSP) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_BSP.OBJ" : $(SOURCE) $(DEP_CPP_R_BSP) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_DRAW.C
DEP_CPP_R_DRA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_DRA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_DRAW.OBJ" : $(SOURCE) $(DEP_CPP_R_DRA) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_DRAW.OBJ" : $(SOURCE) $(DEP_CPP_R_DRA) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_EDGE.C
DEP_CPP_R_EDG=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_EDG=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_EDGE.OBJ" : $(SOURCE) $(DEP_CPP_R_EDG) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_EDGE.OBJ" : $(SOURCE) $(DEP_CPP_R_EDG) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_EFRAG.C
DEP_CPP_R_EFR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_EFR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_EFRAG.OBJ" : $(SOURCE) $(DEP_CPP_R_EFR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_EFRAG.OBJ" : $(SOURCE) $(DEP_CPP_R_EFR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_LIGHT.C
DEP_CPP_R_LIG=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_LIG=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_LIGHT.OBJ" : $(SOURCE) $(DEP_CPP_R_LIG) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_LIGHT.OBJ" : $(SOURCE) $(DEP_CPP_R_LIG) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_local.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_MAIN.C
DEP_CPP_R_MAI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_MAI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_R_MAI) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_R_MAI) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_MISC.C
DEP_CPP_R_MIS=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\shake.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\water.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_MIS=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_MISC.OBJ" : $(SOURCE) $(DEP_CPP_R_MIS) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_MISC.OBJ" : $(SOURCE) $(DEP_CPP_R_MIS) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_PART.C
DEP_CPP_R_PAR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_triangle.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_PAR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_PART.OBJ" : $(SOURCE) $(DEP_CPP_R_PAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_PART.OBJ" : $(SOURCE) $(DEP_CPP_R_PAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\R_PART.OBJ" : $(SOURCE) $(DEP_CPP_R_PAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\R_PART.OBJ" : $(SOURCE) $(DEP_CPP_R_PAR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_shared.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_SKY.C
DEP_CPP_R_SKY=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_SKY=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_SKY.OBJ" : $(SOURCE) $(DEP_CPP_R_SKY) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_SKY.OBJ" : $(SOURCE) $(DEP_CPP_R_SKY) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_SPRITE.C
DEP_CPP_R_SPR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_SPR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_SPRITE.OBJ" : $(SOURCE) $(DEP_CPP_R_SPR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_SPRITE.OBJ" : $(SOURCE) $(DEP_CPP_R_SPR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_studio.c
DEP_CPP_R_STU=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_triangle.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_STU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\r_studio.obj" : $(SOURCE) $(DEP_CPP_R_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\r_studio.obj" : $(SOURCE) $(DEP_CPP_R_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\r_studio.obj" : $(SOURCE) $(DEP_CPP_R_STU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\r_studio.obj" : $(SOURCE) $(DEP_CPP_R_STU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_studio.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_SURF.C
DEP_CPP_R_SUR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_SUR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_SURF.OBJ" : $(SOURCE) $(DEP_CPP_R_SUR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_SURF.OBJ" : $(SOURCE) $(DEP_CPP_R_SUR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_trans.c
DEP_CPP_R_TRA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_TRA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\r_trans.obj" : $(SOURCE) $(DEP_CPP_R_TRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\r_trans.obj" : $(SOURCE) $(DEP_CPP_R_TRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\r_trans.obj" : $(SOURCE) $(DEP_CPP_R_TRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\r_trans.obj" : $(SOURCE) $(DEP_CPP_R_TRA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_trans.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_triangle.c
DEP_CPP_R_TRI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_triangle.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_TRI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\r_triangle.obj" : $(SOURCE) $(DEP_CPP_R_TRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\r_triangle.obj" : $(SOURCE) $(DEP_CPP_R_TRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\r_triangle.obj" : $(SOURCE) $(DEP_CPP_R_TRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\r_triangle.obj" : $(SOURCE) $(DEP_CPP_R_TRI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_triangle.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_VARS.C
DEP_CPP_R_VAR=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_R_VAR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\R_VARS.OBJ" : $(SOURCE) $(DEP_CPP_R_VAR) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\R_VARS.OBJ" : $(SOURCE) $(DEP_CPP_R_VAR) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\render.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\save.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sbar.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCREEN.C
DEP_CPP_SCREE=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SCREE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SCREEN.OBJ" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SCREEN.OBJ" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\screen.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\server.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shake.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\snd_a3d.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SND_DMA.C
DEP_CPP_SND_D=\
	"..\a3dwrapper\a3dwrapper.h"\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\snd_a3d.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SND_D=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SND_DMA.OBJ" : $(SOURCE) $(DEP_CPP_SND_D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SND_DMA.OBJ" : $(SOURCE) $(DEP_CPP_SND_D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\SND_DMA.OBJ" : $(SOURCE) $(DEP_CPP_SND_D) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\SND_DMA.OBJ" : $(SOURCE) $(DEP_CPP_SND_D) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SND_MEM.C
DEP_CPP_SND_M=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SND_M=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SND_MEM.OBJ" : $(SOURCE) $(DEP_CPP_SND_M) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SND_MEM.OBJ" : $(SOURCE) $(DEP_CPP_SND_M) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\SND_MEM.OBJ" : $(SOURCE) $(DEP_CPP_SND_M) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\SND_MEM.OBJ" : $(SOURCE) $(DEP_CPP_SND_M) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SND_MIX.C
DEP_CPP_SND_MI=\
	"..\a3dwrapper\a3dwrapper.h"\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\snd_a3d.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SND_MI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SND_MIX.OBJ" : $(SOURCE) $(DEP_CPP_SND_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SND_MIX.OBJ" : $(SOURCE) $(DEP_CPP_SND_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\SND_MIX.OBJ" : $(SOURCE) $(DEP_CPP_SND_MI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\SND_MIX.OBJ" : $(SOURCE) $(DEP_CPP_SND_MI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SND_WIN.C
DEP_CPP_SND_W=\
	"..\a3dwrapper\a3dwrapper.h"\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\snd_a3d.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SND_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SND_WIN.OBJ" : $(SOURCE) $(DEP_CPP_SND_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SND_WIN.OBJ" : $(SOURCE) $(DEP_CPP_SND_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\SND_WIN.OBJ" : $(SOURCE) $(DEP_CPP_SND_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\SND_WIN.OBJ" : $(SOURCE) $(DEP_CPP_SND_W) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\studio.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SV_MAIN.C
DEP_CPP_SV_MA=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\cmodel.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\pr_edict.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SV_MA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\SV_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_SV_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\SV_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_SV_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\SV_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_SV_MA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\SV_MAIN.OBJ" : $(SOURCE) $(DEP_CPP_SV_MA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_move.c
DEP_CPP_SV_MO=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SV_MO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\sv_move.obj" : $(SOURCE) $(DEP_CPP_SV_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\sv_move.obj" : $(SOURCE) $(DEP_CPP_SV_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\sv_move.obj" : $(SOURCE) $(DEP_CPP_SV_MO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\sv_move.obj" : $(SOURCE) $(DEP_CPP_SV_MO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_phys.c
DEP_CPP_SV_PH=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sv_proto.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SV_PH=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\sv_phys.obj" : $(SOURCE) $(DEP_CPP_SV_PH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\sv_phys.obj" : $(SOURCE) $(DEP_CPP_SV_PH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\sv_phys.obj" : $(SOURCE) $(DEP_CPP_SV_PH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\sv_phys.obj" : $(SOURCE) $(DEP_CPP_SV_PH) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_proto.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_upld.c
DEP_CPP_SV_UP=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SV_UP=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\sv_upld.obj" : $(SOURCE) $(DEP_CPP_SV_UP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\sv_upld.obj" : $(SOURCE) $(DEP_CPP_SV_UP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\sv_upld.obj" : $(SOURCE) $(DEP_CPP_SV_UP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\sv_upld.obj" : $(SOURCE) $(DEP_CPP_SV_UP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_user.c
DEP_CPP_SV_US=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SV_US=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\sv_user.obj" : $(SOURCE) $(DEP_CPP_SV_US) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\sv_user.obj" : $(SOURCE) $(DEP_CPP_SV_US) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\sv_user.obj" : $(SOURCE) $(DEP_CPP_SV_US) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\sv_user.obj" : $(SOURCE) $(DEP_CPP_SV_US) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sys.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sys_win.c
DEP_CPP_SYS_W=\
	"..\common\dll_state.h"\
	"..\common\exefuncs.h"\
	"..\common\gameinfo.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\pr_edict.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_SYS_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\sys_win.obj" : $(SOURCE) $(DEP_CPP_SYS_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\sys_win.obj" : $(SOURCE) $(DEP_CPP_SYS_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\sys_win.obj" : $(SOURCE) $(DEP_CPP_SYS_W) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\sys_win.obj" : $(SOURCE) $(DEP_CPP_SYS_W) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\textures.c
DEP_CPP_TEXTU=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\textures.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_TEXTU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\textures.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\textures.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\textures.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\textures.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\textures.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tmessage.c
DEP_CPP_TMESS=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\cl_demo.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\tmessage.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_TMESS=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\tmessage.obj" : $(SOURCE) $(DEP_CPP_TMESS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\tmessage.obj" : $(SOURCE) $(DEP_CPP_TMESS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

"$(INTDIR)\tmessage.obj" : $(SOURCE) $(DEP_CPP_TMESS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\tmessage.obj" : $(SOURCE) $(DEP_CPP_TMESS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tmessage.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vid.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VID_WIN.C
DEP_CPP_VID_W=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\d_local.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_VID_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\VID_WIN.OBJ" : $(SOURCE) $(DEP_CPP_VID_W) "$(INTDIR)"
   $(CPP) /nologo /MT /W3 /GR /GX /O2 /I "..\common" /I "..\dx6sdk\include" /D\
 "NDEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/"\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\VID_WIN.OBJ" : $(SOURCE) $(DEP_CPP_VID_W) "$(INTDIR)"
   $(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /I\
 "..\dx6sdk\include" /D "_DEBUG" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VIEW.C
DEP_CPP_VIEW_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\shake.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_VIEW_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\VIEW.OBJ" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\VIEW.OBJ" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\VIEW.OBJ" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\VIEW.OBJ" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vmodes.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wad.c
DEP_CPP_WAD_C=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_WAD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\wad.obj" : $(SOURCE) $(DEP_CPP_WAD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\wad.obj" : $(SOURCE) $(DEP_CPP_WAD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\wad.obj" : $(SOURCE) $(DEP_CPP_WAD_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\wad.obj" : $(SOURCE) $(DEP_CPP_WAD_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wad.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\water.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winquake.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\world.c
DEP_CPP_WORLD=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sv_proto.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_WORLD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\world.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wrect.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zone.c
DEP_CPP_ZONE_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\CONSOLE.H"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\d_iface.h"\
	".\draw.h"\
	".\eiface.h"\
	".\GL_MODEL.H"\
	".\GLQUAKE.H"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\model.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_local.h"\
	".\r_shared.h"\
	".\render.h"\
	".\save.h"\
	".\sbar.h"\
	".\screen.h"\
	".\server.h"\
	".\sound.h"\
	".\spritegn.h"\
	".\studio.h"\
	".\sys.h"\
	".\vid.h"\
	".\view.h"\
	".\vmodes.h"\
	".\wad.h"\
	".\winquake.h"\
	".\world.h"\
	".\wrect.h"\
	".\zone.h"\
	{$(INCLUDE)}"\gl\GL.H"\
	
NODEP_CPP_ZONE_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\zone.obj" : $(SOURCE) $(DEP_CPP_ZONE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\zone.obj" : $(SOURCE) $(DEP_CPP_ZONE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\zone.obj" : $(SOURCE) $(DEP_CPP_ZONE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\zone.obj" : $(SOURCE) $(DEP_CPP_ZONE_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zone.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_draw.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_draw
IntDir=.\SoftRelease
InputPath=.\d_draw.asm

"$(INTDIR)\d_draw.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_draw.obj" /Zm /Zi .\d_draw.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_draw
IntDir=.\SoftDebug
InputPath=.\d_draw.asm

"$(INTDIR)\d_draw.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_draw.obj" /Zm /Zi .\d_draw.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_draw16.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_draw16
IntDir=.\SoftRelease
InputPath=.\d_draw16.asm

"$(INTDIR)\d_draw16.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_draw16.obj" /Zm /Zi .\d_draw16.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_draw16
IntDir=.\SoftDebug
InputPath=.\d_draw16.asm

"$(INTDIR)\d_draw16.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_draw16.obj" /Zm /Zi .\d_draw16.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_parta.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_parta
IntDir=.\SoftRelease
InputPath=.\d_parta.asm

"$(INTDIR)\d_parta.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_parta.obj" /Zm /Zi .\d_parta.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_parta
IntDir=.\SoftDebug
InputPath=.\d_parta.asm

"$(INTDIR)\d_parta.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_parta.obj" /Zm /Zi .\d_parta.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_polysa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_polysa
IntDir=.\SoftRelease
InputPath=.\d_polysa.asm

"$(INTDIR)\d_polysa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_polysa.obj" /Zm /Zi .\d_polysa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_polysa
IntDir=.\SoftDebug
InputPath=.\d_polysa.asm

"$(INTDIR)\d_polysa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_polysa.obj" /Zm /Zi .\d_polysa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_spr8.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_spr8
IntDir=.\SoftRelease
InputPath=.\d_spr8.asm

"$(INTDIR)\d_spr8.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_spr8.obj" /Zm /Zi .\d_spr8.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_spr8
IntDir=.\SoftDebug
InputPath=.\d_spr8.asm

"$(INTDIR)\d_spr8.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_spr8.obj" /Zm /Zi .\d_spr8.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\d_varsa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - d_varsa
IntDir=.\SoftRelease
InputPath=.\d_varsa.asm

"$(INTDIR)\d_varsa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_varsa.obj" /Zm /Zi .\d_varsa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - d_varsa
IntDir=.\SoftDebug
InputPath=.\d_varsa.asm

"$(INTDIR)\d_varsa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\d_varsa.obj" /Zm /Zi .\d_varsa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\math.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - math
IntDir=.\SoftRelease
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - math
IntDir=.\SoftDebug
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# Begin Custom Build - math
IntDir=.\GLDebug
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# Begin Custom Build - math
IntDir=.\GLRelease
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_aclipa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - r_aclipa
IntDir=.\SoftRelease
InputPath=.\r_aclipa.asm

"$(INTDIR)\r_aclipa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_aclipa.obj" /Zm /Zi .\r_aclipa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - r_aclipa
IntDir=.\SoftDebug
InputPath=.\r_aclipa.asm

"$(INTDIR)\r_aclipa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_aclipa.obj" /Zm /Zi .\r_aclipa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_aliasa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - r_aliasa
IntDir=.\SoftRelease
InputPath=.\r_aliasa.asm

"$(INTDIR)\r_aliasa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_aliasa.obj" /Zm /Zi .\r_aliasa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - r_aliasa
IntDir=.\SoftDebug
InputPath=.\r_aliasa.asm

"$(INTDIR)\r_aliasa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_aliasa.obj" /Zm /Zi .\r_aliasa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_drawa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - r_drawa
IntDir=.\SoftRelease
InputPath=.\r_drawa.asm

"$(INTDIR)\r_drawa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_drawa.obj" /Zm /Zi .\r_drawa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - r_drawa
IntDir=.\SoftDebug
InputPath=.\r_drawa.asm

"$(INTDIR)\r_drawa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_drawa.obj" /Zm /Zi .\r_drawa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_edgea.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - r_edgea
IntDir=.\SoftRelease
InputPath=.\r_edgea.asm

"$(INTDIR)\r_edgea.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_edgea.obj" /Zm /Zi .\r_edgea.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - r_edgea
IntDir=.\SoftDebug
InputPath=.\r_edgea.asm

"$(INTDIR)\r_edgea.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_edgea.obj" /Zm /Zi .\r_edgea.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_varsa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - r_varsa
IntDir=.\SoftRelease
InputPath=.\r_varsa.asm

"$(INTDIR)\r_varsa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_varsa.obj" /Zm /Zi .\r_varsa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - r_varsa
IntDir=.\SoftDebug
InputPath=.\r_varsa.asm

"$(INTDIR)\r_varsa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\r_varsa.obj" /Zm /Zi .\r_varsa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\snd_mixa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - snd_mixa
IntDir=.\SoftRelease
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - snd_mixa
IntDir=.\SoftDebug
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0
# Begin Custom Build - snd_mixa
IntDir=.\GLDebug
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# Begin Custom Build - snd_mixa
IntDir=.\GLRelease
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\surf8.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - surf8
IntDir=.\SoftRelease
InputPath=.\surf8.asm

"$(INTDIR)\surf8.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\surf8.obj" /Zm /Zi .\surf8.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - surf8
IntDir=.\SoftDebug
InputPath=.\surf8.asm

"$(INTDIR)\surf8.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\surf8.obj" /Zm /Zi .\surf8.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\surf16.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - surf16
IntDir=.\SoftRelease
InputPath=.\surf16.asm

"$(INTDIR)\surf16.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\surf16.obj" /Zm /Zi .\surf16.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - surf16
IntDir=.\SoftDebug
InputPath=.\surf16.asm

"$(INTDIR)\surf16.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\surf16.obj" /Zm /Zi .\surf16.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sys_wina.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - sys_wina
IntDir=.\SoftRelease
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - sys_wina
IntDir=.\SoftDebug
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# PROP Exclude_From_Build 0
# Begin Custom Build - sys_wina
IntDir=.\GLDebug
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# Begin Custom Build - sys_wina
IntDir=.\GLRelease
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\worlda.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - worlda
IntDir=.\SoftRelease
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - worlda
IntDir=.\SoftDebug
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"

# Begin Custom Build - worlda
IntDir=.\GLDebug
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"

# Begin Custom Build - worlda
IntDir=.\GLRelease
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\
"..\a3dwrapper\a3dwrapper.cpp"
DEP_CPP_A3DWR=\
	"..\a3dwrapper\a3dwrapper.h"\
	

!IF  "$(CFG)" == "Quiver - Win32 Release"


"$(INTDIR)\a3dwrapper.obj" : $(SOURCE) $(DEP_CPP_A3DWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"


"$(INTDIR)\a3dwrapper.obj" : $(SOURCE) $(DEP_CPP_A3DWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Debug"


"$(INTDIR)\a3dwrapper.obj" : $(SOURCE) $(DEP_CPP_A3DWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Quiver - Win32 GL Release"


"$(INTDIR)\a3dwrapper.obj" : $(SOURCE) $(DEP_CPP_A3DWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
