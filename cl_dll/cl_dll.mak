# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=cl_dll - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to cl_dll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cl_dll - Win32 Release" && "$(CFG)" != "cl_dll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak" CFG="cl_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cl_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cl_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "cl_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\client.dll"

CLEAN : 
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammohistory.obj"
	-@erase "$(INTDIR)\battery.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\death.obj"
	-@erase "$(INTDIR)\flashlight.obj"
	-@erase "$(INTDIR)\geiger.obj"
	-@erase "$(INTDIR)\health.obj"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\hud_msg.obj"
	-@erase "$(INTDIR)\hud_redraw.obj"
	-@erase "$(INTDIR)\hud_update.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\MOTD.obj"
	-@erase "$(INTDIR)\parsemsg.obj"
	-@erase "$(INTDIR)\saytext.obj"
	-@erase "$(INTDIR)\scoreboard.obj"
	-@erase "$(INTDIR)\train.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(OUTDIR)\client.dll"
	-@erase "$(OUTDIR)\client.exp"
	-@erase "$(OUTDIR)\client.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../engine" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../engine" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/cl_dll.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cl_dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/client.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/client.pdb" /machine:I386 /out:"$(OUTDIR)/client.dll"\
 /implib:"$(OUTDIR)/client.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\ammohistory.obj" \
	"$(INTDIR)\battery.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\death.obj" \
	"$(INTDIR)\flashlight.obj" \
	"$(INTDIR)\geiger.obj" \
	"$(INTDIR)\health.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\hud_msg.obj" \
	"$(INTDIR)\hud_redraw.obj" \
	"$(INTDIR)\hud_update.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\MOTD.obj" \
	"$(INTDIR)\parsemsg.obj" \
	"$(INTDIR)\saytext.obj" \
	"$(INTDIR)\scoreboard.obj" \
	"$(INTDIR)\train.obj" \
	"$(INTDIR)\util.obj"

"$(OUTDIR)\client.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\client.dll"

CLEAN : 
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammohistory.obj"
	-@erase "$(INTDIR)\battery.obj"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\death.obj"
	-@erase "$(INTDIR)\flashlight.obj"
	-@erase "$(INTDIR)\geiger.obj"
	-@erase "$(INTDIR)\health.obj"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\hud_msg.obj"
	-@erase "$(INTDIR)\hud_redraw.obj"
	-@erase "$(INTDIR)\hud_update.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\MOTD.obj"
	-@erase "$(INTDIR)\parsemsg.obj"
	-@erase "$(INTDIR)\saytext.obj"
	-@erase "$(INTDIR)\scoreboard.obj"
	-@erase "$(INTDIR)\train.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\client.dll"
	-@erase "$(OUTDIR)\client.exp"
	-@erase "$(OUTDIR)\client.ilk"
	-@erase "$(OUTDIR)\client.lib"
	-@erase "$(OUTDIR)\client.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../engine" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "../engine" /D "WIN32" /D "_DEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/cl_dll.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cl_dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/client.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/client.pdb" /debug /machine:I386 /out:"$(OUTDIR)/client.dll"\
 /implib:"$(OUTDIR)/client.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\ammohistory.obj" \
	"$(INTDIR)\battery.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\death.obj" \
	"$(INTDIR)\flashlight.obj" \
	"$(INTDIR)\geiger.obj" \
	"$(INTDIR)\health.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\hud_msg.obj" \
	"$(INTDIR)\hud_redraw.obj" \
	"$(INTDIR)\hud_update.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\MOTD.obj" \
	"$(INTDIR)\parsemsg.obj" \
	"$(INTDIR)\saytext.obj" \
	"$(INTDIR)\scoreboard.obj" \
	"$(INTDIR)\train.obj" \
	"$(INTDIR)\util.obj"

"$(OUTDIR)\client.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "cl_dll - Win32 Release"
# Name "cl_dll - Win32 Debug"

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\util_vector.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ammo.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ammohistory.cpp
DEP_CPP_AMMOH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\ammohistory.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\ammohistory.obj" : $(SOURCE) $(DEP_CPP_AMMOH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ammohistory.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\battery.cpp
DEP_CPP_BATTE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\battery.obj" : $(SOURCE) $(DEP_CPP_BATTE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_int.cpp
DEP_CPP_CDLL_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\cdll_int.obj" : $(SOURCE) $(DEP_CPP_CDLL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cl_dll.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\death.cpp
DEP_CPP_DEATH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\death.obj" : $(SOURCE) $(DEP_CPP_DEATH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\flashlight.cpp
DEP_CPP_FLASH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\flashlight.obj" : $(SOURCE) $(DEP_CPP_FLASH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\geiger.cpp
DEP_CPP_GEIGE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\geiger.obj" : $(SOURCE) $(DEP_CPP_GEIGE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\health.cpp
DEP_CPP_HEALT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\health.obj" : $(SOURCE) $(DEP_CPP_HEALT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\health.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud.cpp
DEP_CPP_HUD_C=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\hud.obj" : $(SOURCE) $(DEP_CPP_HUD_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud_msg.cpp
DEP_CPP_HUD_M=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\hud_msg.obj" : $(SOURCE) $(DEP_CPP_HUD_M) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud_redraw.cpp
DEP_CPP_HUD_R=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\hud_redraw.obj" : $(SOURCE) $(DEP_CPP_HUD_R) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hud_update.cpp
DEP_CPP_HUD_U=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\hud_update.obj" : $(SOURCE) $(DEP_CPP_HUD_U) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\message.cpp
DEP_CPP_MESSA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MOTD.cpp
DEP_CPP_MOTD_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\MOTD.obj" : $(SOURCE) $(DEP_CPP_MOTD_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\parsemsg.cpp

"$(INTDIR)\parsemsg.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\parsemsg.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\saytext.cpp
DEP_CPP_SAYTE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\saytext.obj" : $(SOURCE) $(DEP_CPP_SAYTE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\scoreboard.cpp
DEP_CPP_SCORE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\scoreboard.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\train.cpp
DEP_CPP_TRAIN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\train.obj" : $(SOURCE) $(DEP_CPP_TRAIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp
DEP_CPP_UTIL_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.h

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ammo.cpp
DEP_CPP_AMMO_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\cdll_int.h"\
	"..\engine\cvardef.h"\
	".\ammo.h"\
	".\ammohistory.h"\
	".\cl_dll.h"\
	".\health.h"\
	".\hud.h"\
	".\parsemsg.h"\
	".\util.h"\
	".\util_vector.h"\
	

"$(INTDIR)\ammo.obj" : $(SOURCE) $(DEP_CPP_AMMO_) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
