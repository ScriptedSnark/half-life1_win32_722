# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Quiver - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Quiver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Quiver - Win32 Release" && "$(CFG)" != "Quiver - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quiver.mak" CFG="Quiver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quiver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Quiver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Quiver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
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
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\Cl_demo.obj"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\Cl_main.obj"
	-@erase "$(INTDIR)\Cl_parse.obj"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\Cl_tent.obj"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\Console.obj"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\Gl_draw.obj"
	-@erase "$(INTDIR)\Gl_model.obj"
	-@erase "$(INTDIR)\gl_refrag.obj"
	-@erase "$(INTDIR)\gl_rlight.obj"
	-@erase "$(INTDIR)\Gl_rmain.obj"
	-@erase "$(INTDIR)\Gl_rmisc.obj"
	-@erase "$(INTDIR)\Gl_rsurf.obj"
	-@erase "$(INTDIR)\gl_screen.obj"
	-@erase "$(INTDIR)\Gl_vidnt.obj"
	-@erase "$(INTDIR)\Gl_warp.obj"
	-@erase "$(INTDIR)\glHud.obj"
	-@erase "$(INTDIR)\glide.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_camera.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\Keys.obj"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\opengl2d3d.obj"
	-@erase "$(INTDIR)\pe_export.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\R_part.obj"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\Snd_dma.obj"
	-@erase "$(INTDIR)\Snd_mem.obj"
	-@erase "$(INTDIR)\Snd_mix.obj"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\Snd_win.obj"
	-@erase "$(INTDIR)\sv_main.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_send.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\vid_win.obj"
	-@erase "$(INTDIR)\View.obj"
	-@erase "$(INTDIR)\wad.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\worlda.obj"
	-@erase "$(INTDIR)\zone.obj"
	-@erase "$(OUTDIR)\hw.dll"
	-@erase "$(OUTDIR)\hw.exp"
	-@erase "$(OUTDIR)\hw.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\common" /D "NDEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\common" /D "NDEBUG" /D "GLQUAKE" /D\
 "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)/Quiver.pch"\
 /YX /Fo"$(INTDIR)/" /c 
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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"GLRelease/hw.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/hw.pdb" /machine:I386 /out:"$(OUTDIR)/hw.dll"\
 /implib:"$(OUTDIR)/hw.lib" 
LINK32_OBJS= \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\Cl_demo.obj" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\Cl_main.obj" \
	"$(INTDIR)\Cl_parse.obj" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\Cl_tent.obj" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\Console.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\Gl_draw.obj" \
	"$(INTDIR)\Gl_model.obj" \
	"$(INTDIR)\gl_refrag.obj" \
	"$(INTDIR)\gl_rlight.obj" \
	"$(INTDIR)\Gl_rmain.obj" \
	"$(INTDIR)\Gl_rmisc.obj" \
	"$(INTDIR)\Gl_rsurf.obj" \
	"$(INTDIR)\gl_screen.obj" \
	"$(INTDIR)\Gl_vidnt.obj" \
	"$(INTDIR)\Gl_warp.obj" \
	"$(INTDIR)\glHud.obj" \
	"$(INTDIR)\glide.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_camera.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\Keys.obj" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\opengl2d3d.obj" \
	"$(INTDIR)\pe_export.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\R_part.obj" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\Snd_dma.obj" \
	"$(INTDIR)\Snd_mem.obj" \
	"$(INTDIR)\Snd_mix.obj" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\Snd_win.obj" \
	"$(INTDIR)\sv_main.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_send.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\vid_win.obj" \
	"$(INTDIR)\View.obj" \
	"$(INTDIR)\wad.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\worlda.obj" \
	"$(INTDIR)\zone.obj"

"$(OUTDIR)\hw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# PROP Output_Dir "GLDebug"
# PROP Intermediate_Dir "GLDebug"
# PROP Target_Dir ""
OUTDIR=.\GLDebug
INTDIR=.\GLDebug

ALL : "$(OUTDIR)\hw.dll"

CLEAN : 
	-@erase "$(INTDIR)\buildnum.obj"
	-@erase "$(INTDIR)\cdll_exp.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\chase.obj"
	-@erase "$(INTDIR)\cl_cam.obj"
	-@erase "$(INTDIR)\Cl_demo.obj"
	-@erase "$(INTDIR)\cl_draw.obj"
	-@erase "$(INTDIR)\cl_ents.obj"
	-@erase "$(INTDIR)\cl_input.obj"
	-@erase "$(INTDIR)\Cl_main.obj"
	-@erase "$(INTDIR)\Cl_parse.obj"
	-@erase "$(INTDIR)\cl_pred.obj"
	-@erase "$(INTDIR)\Cl_tent.obj"
	-@erase "$(INTDIR)\cmd.obj"
	-@erase "$(INTDIR)\cmodel.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\conproc.obj"
	-@erase "$(INTDIR)\Console.obj"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\cvar.obj"
	-@erase "$(INTDIR)\Gl_draw.obj"
	-@erase "$(INTDIR)\Gl_model.obj"
	-@erase "$(INTDIR)\gl_refrag.obj"
	-@erase "$(INTDIR)\gl_rlight.obj"
	-@erase "$(INTDIR)\Gl_rmain.obj"
	-@erase "$(INTDIR)\Gl_rmisc.obj"
	-@erase "$(INTDIR)\Gl_rsurf.obj"
	-@erase "$(INTDIR)\gl_screen.obj"
	-@erase "$(INTDIR)\Gl_vidnt.obj"
	-@erase "$(INTDIR)\Gl_warp.obj"
	-@erase "$(INTDIR)\glHud.obj"
	-@erase "$(INTDIR)\glide.obj"
	-@erase "$(INTDIR)\ha3d.obj"
	-@erase "$(INTDIR)\hashpak.obj"
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host_cmd.obj"
	-@erase "$(INTDIR)\HUD.obj"
	-@erase "$(INTDIR)\in_camera.obj"
	-@erase "$(INTDIR)\in_win.obj"
	-@erase "$(INTDIR)\Keys.obj"
	-@erase "$(INTDIR)\l_studio.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\net_chan.obj"
	-@erase "$(INTDIR)\net_ws.obj"
	-@erase "$(INTDIR)\opengl2d3d.obj"
	-@erase "$(INTDIR)\pe_export.obj"
	-@erase "$(INTDIR)\pmove.obj"
	-@erase "$(INTDIR)\pmovetst.obj"
	-@erase "$(INTDIR)\pr_cmds.obj"
	-@erase "$(INTDIR)\pr_edict.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\R_part.obj"
	-@erase "$(INTDIR)\r_studio.obj"
	-@erase "$(INTDIR)\r_trans.obj"
	-@erase "$(INTDIR)\r_triangle.obj"
	-@erase "$(INTDIR)\Snd_dma.obj"
	-@erase "$(INTDIR)\Snd_mem.obj"
	-@erase "$(INTDIR)\Snd_mix.obj"
	-@erase "$(INTDIR)\snd_mixa.obj"
	-@erase "$(INTDIR)\Snd_win.obj"
	-@erase "$(INTDIR)\sv_main.obj"
	-@erase "$(INTDIR)\sv_phys.obj"
	-@erase "$(INTDIR)\sv_send.obj"
	-@erase "$(INTDIR)\sv_upld.obj"
	-@erase "$(INTDIR)\sv_user.obj"
	-@erase "$(INTDIR)\sys_win.obj"
	-@erase "$(INTDIR)\sys_wina.obj"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\tmessage.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\vid_win.obj"
	-@erase "$(INTDIR)\View.obj"
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

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /D "_DEBUG" /D "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\common" /D "_DEBUG" /D\
 "GLQUAKE" /D "__USEA3D" /D "__A3D_GEOM" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Quiver.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"GLDebug/hw.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)/hw.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/hw.dll" /implib:"$(OUTDIR)/hw.lib" 
LINK32_OBJS= \
	"$(INTDIR)\buildnum.obj" \
	"$(INTDIR)\cdll_exp.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\chase.obj" \
	"$(INTDIR)\cl_cam.obj" \
	"$(INTDIR)\Cl_demo.obj" \
	"$(INTDIR)\cl_draw.obj" \
	"$(INTDIR)\cl_ents.obj" \
	"$(INTDIR)\cl_input.obj" \
	"$(INTDIR)\Cl_main.obj" \
	"$(INTDIR)\Cl_parse.obj" \
	"$(INTDIR)\cl_pred.obj" \
	"$(INTDIR)\Cl_tent.obj" \
	"$(INTDIR)\cmd.obj" \
	"$(INTDIR)\cmodel.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\conproc.obj" \
	"$(INTDIR)\Console.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\cvar.obj" \
	"$(INTDIR)\Gl_draw.obj" \
	"$(INTDIR)\Gl_model.obj" \
	"$(INTDIR)\gl_refrag.obj" \
	"$(INTDIR)\gl_rlight.obj" \
	"$(INTDIR)\Gl_rmain.obj" \
	"$(INTDIR)\Gl_rmisc.obj" \
	"$(INTDIR)\Gl_rsurf.obj" \
	"$(INTDIR)\gl_screen.obj" \
	"$(INTDIR)\Gl_vidnt.obj" \
	"$(INTDIR)\Gl_warp.obj" \
	"$(INTDIR)\glHud.obj" \
	"$(INTDIR)\glide.obj" \
	"$(INTDIR)\ha3d.obj" \
	"$(INTDIR)\hashpak.obj" \
	"$(INTDIR)\host.obj" \
	"$(INTDIR)\host_cmd.obj" \
	"$(INTDIR)\HUD.obj" \
	"$(INTDIR)\in_camera.obj" \
	"$(INTDIR)\in_win.obj" \
	"$(INTDIR)\Keys.obj" \
	"$(INTDIR)\l_studio.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\net_chan.obj" \
	"$(INTDIR)\net_ws.obj" \
	"$(INTDIR)\opengl2d3d.obj" \
	"$(INTDIR)\pe_export.obj" \
	"$(INTDIR)\pmove.obj" \
	"$(INTDIR)\pmovetst.obj" \
	"$(INTDIR)\pr_cmds.obj" \
	"$(INTDIR)\pr_edict.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\R_part.obj" \
	"$(INTDIR)\r_studio.obj" \
	"$(INTDIR)\r_trans.obj" \
	"$(INTDIR)\r_triangle.obj" \
	"$(INTDIR)\Snd_dma.obj" \
	"$(INTDIR)\Snd_mem.obj" \
	"$(INTDIR)\Snd_mix.obj" \
	"$(INTDIR)\snd_mixa.obj" \
	"$(INTDIR)\Snd_win.obj" \
	"$(INTDIR)\sv_main.obj" \
	"$(INTDIR)\sv_phys.obj" \
	"$(INTDIR)\sv_send.obj" \
	"$(INTDIR)\sv_upld.obj" \
	"$(INTDIR)\sv_user.obj" \
	"$(INTDIR)\sys_win.obj" \
	"$(INTDIR)\sys_wina.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\tmessage.obj" \
	"$(INTDIR)\vid_win.obj" \
	"$(INTDIR)\View.obj" \
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

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\zone.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\anorm_dots.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\anorms.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\beamdef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bothdefs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bspfile.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_BUILD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\buildnum.obj" : $(SOURCE) $(DEP_CPP_BUILD) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CDLL_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cdll_exp.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\hud_handlers.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CDLL_I=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_I) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_int.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CHASE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\chase.obj" : $(SOURCE) $(DEP_CPP_CHASE) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_CA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cl_cam.obj" : $(SOURCE) $(DEP_CPP_CL_CA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cl_demo.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_DE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Cl_demo.obj" : $(SOURCE) $(DEP_CPP_CL_DE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_demo.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_DR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cl_draw.obj" : $(SOURCE) $(DEP_CPP_CL_DR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_draw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\cl_tent.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_EN=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cl_ents.obj" : $(SOURCE) $(DEP_CPP_CL_EN) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_IN=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cl_input.obj" : $(SOURCE) $(DEP_CPP_CL_IN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cl_main.c
DEP_CPP_CL_MA=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_MA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Cl_main.obj" : $(SOURCE) $(DEP_CPP_CL_MA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cl_parse.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_trans.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_PA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Cl_parse.obj" : $(SOURCE) $(DEP_CPP_CL_PA) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_PR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cl_pred.obj" : $(SOURCE) $(DEP_CPP_CL_PR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cl_tent.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CL_TE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Cl_tent.obj" : $(SOURCE) $(DEP_CPP_CL_TE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_tent.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\client.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CMD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cmd.obj" : $(SOURCE) $(DEP_CPP_CMD_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmd.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CMODE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cmodel.obj" : $(SOURCE) $(DEP_CPP_CMODE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cmodel.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\color.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_COMMO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\common.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CONPR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\conproc.obj" : $(SOURCE) $(DEP_CPP_CONPR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\conproc.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Console.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CONSO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Console.obj" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Console.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\const.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CRC_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\crc.obj" : $(SOURCE) $(DEP_CPP_CRC_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\crc.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cshift.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\custom.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\customentity.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_CVAR_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\cvar.obj" : $(SOURCE) $(DEP_CPP_CVAR_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cvar.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cvardef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\decal.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\draw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dummyfile.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\eiface.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_draw.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\opengl2d3d.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_DR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_draw.obj" : $(SOURCE) $(DEP_CPP_GL_DR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_hw.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_model.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\gl_water.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_MO=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_model.obj" : $(SOURCE) $(DEP_CPP_GL_MO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_model.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_RE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\gl_refrag.obj" : $(SOURCE) $(DEP_CPP_GL_RE) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_RL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\gl_rlight.obj" : $(SOURCE) $(DEP_CPP_GL_RL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_rmain.c
DEP_CPP_GL_RM=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_RM=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_rmain.obj" : $(SOURCE) $(DEP_CPP_GL_RM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_rmisc.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_RMI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_rmisc.obj" : $(SOURCE) $(DEP_CPP_GL_RMI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_rsurf.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\gl_water.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_RS=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_rsurf.obj" : $(SOURCE) $(DEP_CPP_GL_RS) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_SC=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\gl_screen.obj" : $(SOURCE) $(DEP_CPP_GL_SC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_vidnt.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\gl_hw.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\opengl2d3d.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_VI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_vidnt.obj" : $(SOURCE) $(DEP_CPP_GL_VI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gl_warp.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\gl_warp_sin.h"\
	".\gl_water.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GL_WA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Gl_warp.obj" : $(SOURCE) $(DEP_CPP_GL_WA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_warp_sin.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gl_water.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GLHUD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\glHud.obj" : $(SOURCE) $(DEP_CPP_GLHUD) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_GLIDE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\glide.obj" : $(SOURCE) $(DEP_CPP_GLIDE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Glquake.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ha3d.c
DEP_CPP_HA3D_=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\a3d.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_HA3D_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\ha3d.obj" : $(SOURCE) $(DEP_CPP_HA3D_) "$(INTDIR)"


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
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_HASHP=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\hashpak.obj" : $(SOURCE) $(DEP_CPP_HASHP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hashpak.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_HOST_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\host.obj" : $(SOURCE) $(DEP_CPP_HOST_) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_HOST_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\host_cmd.obj" : $(SOURCE) $(DEP_CPP_HOST_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\host_cmd.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_HUD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\HUD.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud_handlers.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\in_camera.c

"$(INTDIR)\in_camera.obj" : $(SOURCE) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_IN_WI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\in_win.obj" : $(SOURCE) $(DEP_CPP_IN_WI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\input.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Keys.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_KEYS_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Keys.obj" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\keys.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_L_STU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\l_studio.obj" : $(SOURCE) $(DEP_CPP_L_STU) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_MATHL=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\mathlib.obj" : $(SOURCE) $(DEP_CPP_MATHL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mathlib.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\modelgen.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\net.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_NET_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\net_chan.obj" : $(SOURCE) $(DEP_CPP_NET_C) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_NET_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\net_ws.obj" : $(SOURCE) $(DEP_CPP_NET_W) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\opengl2d3d.c
DEP_CPP_OPENG=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\opengl2d3d.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_OPENG=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\opengl2d3d.obj" : $(SOURCE) $(DEP_CPP_OPENG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\opengl2d3d.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pe_export.c
DEP_CPP_PE_EX=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PE_EX=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\pe_export.obj" : $(SOURCE) $(DEP_CPP_PE_EX) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PMOVE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\pmove.obj" : $(SOURCE) $(DEP_CPP_PMOVE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pmove.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PMOVET=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\pmovetst.obj" : $(SOURCE) $(DEP_CPP_PMOVET) "$(INTDIR)"


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
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PR_CM=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\pr_cmds.obj" : $(SOURCE) $(DEP_CPP_PR_CM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_cmds.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pr_dlls.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\profile.c
DEP_CPP_PROFI=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PROFI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\profile.obj" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\profile.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\progdefs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Progs.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\protocol.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_QGL_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\qgl.obj" : $(SOURCE) $(DEP_CPP_QGL_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\qgl.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\quakedef.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_local.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\R_part.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_triangle.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_R_PAR=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\R_part.obj" : $(SOURCE) $(DEP_CPP_R_PAR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_shared.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\customentity.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_R_STU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\r_studio.obj" : $(SOURCE) $(DEP_CPP_R_STU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_studio.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_studio.h"\
	".\r_trans.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_R_TRA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\r_trans.obj" : $(SOURCE) $(DEP_CPP_R_TRA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_trans.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\r_triangle.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_R_TRI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\r_triangle.obj" : $(SOURCE) $(DEP_CPP_R_TRI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\r_triangle.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\render.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sbar.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\screen.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\server.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shake.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Snd_dma.c
DEP_CPP_SND_D=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\a3d.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SND_D=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Snd_dma.obj" : $(SOURCE) $(DEP_CPP_SND_D) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Snd_mem.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SND_M=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Snd_mem.obj" : $(SOURCE) $(DEP_CPP_SND_M) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Snd_mix.c
DEP_CPP_SND_MI=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\a3d.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_cmds.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SND_MI=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Snd_mix.obj" : $(SOURCE) $(DEP_CPP_SND_MI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Snd_win.c
DEP_CPP_SND_W=\
	"..\common\dll_state.h"\
	"..\common\platform.h"\
	"..\common\qfont.h"\
	".\a3d.h"\
	".\beamdef.h"\
	".\bothdefs.h"\
	".\bspfile.h"\
	".\cdll_int.h"\
	".\client.h"\
	".\cmd.h"\
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SND_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\Snd_win.obj" : $(SOURCE) $(DEP_CPP_SND_W) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spritegn.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\studio.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_main.c
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
	".\color.h"\
	".\common.h"\
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SV_MA=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sv_main.obj" : $(SOURCE) $(DEP_CPP_SV_MA) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pmove.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SV_PH=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sv_phys.obj" : $(SOURCE) $(DEP_CPP_SV_PH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sv_send.c
DEP_CPP_SV_SE=\
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SV_SE=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sv_send.obj" : $(SOURCE) $(DEP_CPP_SV_SE) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\decal.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\hashpak.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SV_UP=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sv_upld.obj" : $(SOURCE) $(DEP_CPP_SV_UP) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SV_US=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sv_user.obj" : $(SOURCE) $(DEP_CPP_SV_US) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sys.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\profile.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_SYS_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\sys_win.obj" : $(SOURCE) $(DEP_CPP_SYS_W) "$(INTDIR)"


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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_TEXTU=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\textures.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\textures.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_TMESS=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\tmessage.obj" : $(SOURCE) $(DEP_CPP_TMESS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tmessage.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vid.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vid_win.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_VID_W=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\vid_win.obj" : $(SOURCE) $(DEP_CPP_VID_W) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\View.c
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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_VIEW_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\View.obj" : $(SOURCE) $(DEP_CPP_VIEW_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\view.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vmodes.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_WAD_C=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\wad.obj" : $(SOURCE) $(DEP_CPP_WAD_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wad.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winquake.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_WORLD=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\world.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wrect.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_ZONE_=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\zone.obj" : $(SOURCE) $(DEP_CPP_ZONE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\a3d.h

!IF  "$(CFG)" == "Quiver - Win32 Release"

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\snd_mixa.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - snd_mixa
IntDir=.\GLRelease
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - snd_mixa
IntDir=.\GLDebug
InputPath=.\snd_mixa.asm

"$(INTDIR)\snd_mixa.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\snd_mixa.obj" /Zm /Zi .\snd_mixa.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sys_wina.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - sys_wina
IntDir=.\GLRelease
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - sys_wina
IntDir=.\GLDebug
InputPath=.\sys_wina.asm

"$(INTDIR)\sys_wina.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\sys_wina.obj" /Zm /Zi .\sys_wina.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\math.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - math
IntDir=.\GLRelease
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - math
IntDir=.\GLDebug
InputPath=.\math.asm

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\math.obj" /Zm /Zi .\math.asm

# End Custom Build

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\worlda.asm

!IF  "$(CFG)" == "Quiver - Win32 Release"

# Begin Custom Build - worlda
IntDir=.\GLRelease
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "Quiver - Win32 Debug"

# Begin Custom Build - worlda
IntDir=.\GLDebug
InputPath=.\worlda.asm

"$(INTDIR)\worlda.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   ml /c /Cp /coff /Fo"$(INTDIR)\worlda.obj" /Zm /Zi .\worlda.asm

# End Custom Build

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
	".\Console.h"\
	".\const.h"\
	".\crc.h"\
	".\cshift.h"\
	".\custom.h"\
	".\cvar.h"\
	".\cvardef.h"\
	".\draw.h"\
	".\eiface.h"\
	".\Gl_model.h"\
	".\Glquake.h"\
	".\host_cmd.h"\
	".\input.h"\
	".\keys.h"\
	".\mathlib.h"\
	".\modelgen.h"\
	".\net.h"\
	".\pr_dlls.h"\
	".\progdefs.h"\
	".\Progs.h"\
	".\protocol.h"\
	".\qgl.h"\
	".\quakedef.h"\
	".\r_shared.h"\
	".\render.h"\
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
	{$(INCLUDE)}"\GL\gl.h"\
	
NODEP_CPP_PR_ED=\
	".\cmdlib.h"\
	".\lbmlib.h"\
	".\scriplib.h"\
	".\trilib.h"\
	

"$(INTDIR)\pr_edict.obj" : $(SOURCE) $(DEP_CPP_PR_ED) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
