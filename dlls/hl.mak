# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=hl - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to hl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hl - Win32 Release" && "$(CFG)" != "hl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "hl.mak" CFG="hl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "hl - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Releasehl"
# PROP Intermediate_Dir "Releasehl"
# PROP Target_Dir ""
OUTDIR=.\Releasehl
INTDIR=.\Releasehl

ALL : "$(OUTDIR)\hl.dll"

CLEAN : 
	-@erase "$(INTDIR)\aflock.obj"
	-@erase "$(INTDIR)\agrunt.obj"
	-@erase "$(INTDIR)\airtank.obj"
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\barnacle.obj"
	-@erase "$(INTDIR)\barney.obj"
	-@erase "$(INTDIR)\bigmomma.obj"
	-@erase "$(INTDIR)\bloater.obj"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bullsquid.obj"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\defaultai.obj"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\flyingmonster.obj"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_tank.obj"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gargantua.obj"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\genericmonster.obj"
	-@erase "$(INTDIR)\ggrenade.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\gman.obj"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_battery.obj"
	-@erase "$(INTDIR)\h_cine.obj"
	-@erase "$(INTDIR)\h_cycler.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\hassassin.obj"
	-@erase "$(INTDIR)\headcrab.obj"
	-@erase "$(INTDIR)\healthkit.obj"
	-@erase "$(INTDIR)\hgrunt.obj"
	-@erase "$(INTDIR)\hornet.obj"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\houndeye.obj"
	-@erase "$(INTDIR)\ichthyosaur.obj"
	-@erase "$(INTDIR)\islave.obj"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\leech.obj"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\monstermaker.obj"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\mortar.obj"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\nihilanth.obj"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\osprey.obj"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\rat.obj"
	-@erase "$(INTDIR)\roach.obj"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\schedule.obj"
	-@erase "$(INTDIR)\scientist.obj"
	-@erase "$(INTDIR)\scripted.obj"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\soundent.obj"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\squadmonster.obj"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\talkmonster.obj"
	-@erase "$(INTDIR)\tempmonster.obj"
	-@erase "$(INTDIR)\tentacle.obj"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\turret.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\xen.obj"
	-@erase "$(INTDIR)\zombie.obj"
	-@erase "$(OUTDIR)\hl.dll"
	-@erase "$(OUTDIR)\hl.exp"
	-@erase "$(OUTDIR)\hl.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../engine" /I "../common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../engine" /I "../common" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL"\
 /Fp"$(INTDIR)/hl.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Releasehl/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/hl.pdb" /machine:I386 /def:".\hl.def" /out:"$(OUTDIR)/hl.dll"\
 /implib:"$(OUTDIR)/hl.lib" 
DEF_FILE= \
	".\hl.def"
LINK32_OBJS= \
	"$(INTDIR)\aflock.obj" \
	"$(INTDIR)\agrunt.obj" \
	"$(INTDIR)\airtank.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\barnacle.obj" \
	"$(INTDIR)\barney.obj" \
	"$(INTDIR)\bigmomma.obj" \
	"$(INTDIR)\bloater.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\bullsquid.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\crossbow.obj" \
	"$(INTDIR)\crowbar.obj" \
	"$(INTDIR)\defaultai.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\egon.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\flyingmonster.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\func_tank.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\gargantua.obj" \
	"$(INTDIR)\gauss.obj" \
	"$(INTDIR)\genericmonster.obj" \
	"$(INTDIR)\ggrenade.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\glock.obj" \
	"$(INTDIR)\gman.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_battery.obj" \
	"$(INTDIR)\h_cine.obj" \
	"$(INTDIR)\h_cycler.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\handgrenade.obj" \
	"$(INTDIR)\hassassin.obj" \
	"$(INTDIR)\headcrab.obj" \
	"$(INTDIR)\healthkit.obj" \
	"$(INTDIR)\hgrunt.obj" \
	"$(INTDIR)\hornet.obj" \
	"$(INTDIR)\hornetgun.obj" \
	"$(INTDIR)\houndeye.obj" \
	"$(INTDIR)\ichthyosaur.obj" \
	"$(INTDIR)\islave.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\leech.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\monstermaker.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\mortar.obj" \
	"$(INTDIR)\mp5.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\nihilanth.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\osprey.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\python.obj" \
	"$(INTDIR)\rat.obj" \
	"$(INTDIR)\roach.obj" \
	"$(INTDIR)\rpg.obj" \
	"$(INTDIR)\satchel.obj" \
	"$(INTDIR)\schedule.obj" \
	"$(INTDIR)\scientist.obj" \
	"$(INTDIR)\scripted.obj" \
	"$(INTDIR)\shotgun.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\soundent.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\squadmonster.obj" \
	"$(INTDIR)\squeakgrenade.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\talkmonster.obj" \
	"$(INTDIR)\tempmonster.obj" \
	"$(INTDIR)\tentacle.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\tripmine.obj" \
	"$(INTDIR)\turret.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\xen.obj" \
	"$(INTDIR)\zombie.obj"

"$(OUTDIR)\hl.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debughl"
# PROP Intermediate_Dir "Debughl"
# PROP Target_Dir ""
OUTDIR=.\Debughl
INTDIR=.\Debughl

ALL : "$(OUTDIR)\hl.dll"

CLEAN : 
	-@erase "$(INTDIR)\aflock.obj"
	-@erase "$(INTDIR)\agrunt.obj"
	-@erase "$(INTDIR)\airtank.obj"
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\barnacle.obj"
	-@erase "$(INTDIR)\barney.obj"
	-@erase "$(INTDIR)\bigmomma.obj"
	-@erase "$(INTDIR)\bloater.obj"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bullsquid.obj"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\defaultai.obj"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\flyingmonster.obj"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_tank.obj"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gargantua.obj"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\genericmonster.obj"
	-@erase "$(INTDIR)\ggrenade.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\gman.obj"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_battery.obj"
	-@erase "$(INTDIR)\h_cine.obj"
	-@erase "$(INTDIR)\h_cycler.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\hassassin.obj"
	-@erase "$(INTDIR)\headcrab.obj"
	-@erase "$(INTDIR)\healthkit.obj"
	-@erase "$(INTDIR)\hgrunt.obj"
	-@erase "$(INTDIR)\hornet.obj"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\houndeye.obj"
	-@erase "$(INTDIR)\ichthyosaur.obj"
	-@erase "$(INTDIR)\islave.obj"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\leech.obj"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\monstermaker.obj"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\mortar.obj"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\nihilanth.obj"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\osprey.obj"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\rat.obj"
	-@erase "$(INTDIR)\roach.obj"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\schedule.obj"
	-@erase "$(INTDIR)\scientist.obj"
	-@erase "$(INTDIR)\scripted.obj"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\soundent.obj"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\squadmonster.obj"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\talkmonster.obj"
	-@erase "$(INTDIR)\tempmonster.obj"
	-@erase "$(INTDIR)\tentacle.obj"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\turret.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\xen.obj"
	-@erase "$(INTDIR)\zombie.obj"
	-@erase "$(OUTDIR)\hl.dll"
	-@erase "$(OUTDIR)\hl.exp"
	-@erase "$(OUTDIR)\hl.ilk"
	-@erase "$(OUTDIR)\hl.lib"
	-@erase "$(OUTDIR)\hl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../engine" /I "../common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "../engine" /I "../common" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D\
 "VALVE_DLL" /Fp"$(INTDIR)/hl.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debughl/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/hl.pdb" /debug /machine:I386 /def:".\hl.def"\
 /out:"$(OUTDIR)/hl.dll" /implib:"$(OUTDIR)/hl.lib" 
DEF_FILE= \
	".\hl.def"
LINK32_OBJS= \
	"$(INTDIR)\aflock.obj" \
	"$(INTDIR)\agrunt.obj" \
	"$(INTDIR)\airtank.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\barnacle.obj" \
	"$(INTDIR)\barney.obj" \
	"$(INTDIR)\bigmomma.obj" \
	"$(INTDIR)\bloater.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\bullsquid.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\crossbow.obj" \
	"$(INTDIR)\crowbar.obj" \
	"$(INTDIR)\defaultai.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\egon.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\flyingmonster.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\func_tank.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\gargantua.obj" \
	"$(INTDIR)\gauss.obj" \
	"$(INTDIR)\genericmonster.obj" \
	"$(INTDIR)\ggrenade.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\glock.obj" \
	"$(INTDIR)\gman.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_battery.obj" \
	"$(INTDIR)\h_cine.obj" \
	"$(INTDIR)\h_cycler.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\handgrenade.obj" \
	"$(INTDIR)\hassassin.obj" \
	"$(INTDIR)\headcrab.obj" \
	"$(INTDIR)\healthkit.obj" \
	"$(INTDIR)\hgrunt.obj" \
	"$(INTDIR)\hornet.obj" \
	"$(INTDIR)\hornetgun.obj" \
	"$(INTDIR)\houndeye.obj" \
	"$(INTDIR)\ichthyosaur.obj" \
	"$(INTDIR)\islave.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\leech.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\monstermaker.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\mortar.obj" \
	"$(INTDIR)\mp5.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\nihilanth.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\osprey.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\python.obj" \
	"$(INTDIR)\rat.obj" \
	"$(INTDIR)\roach.obj" \
	"$(INTDIR)\rpg.obj" \
	"$(INTDIR)\satchel.obj" \
	"$(INTDIR)\schedule.obj" \
	"$(INTDIR)\scientist.obj" \
	"$(INTDIR)\scripted.obj" \
	"$(INTDIR)\shotgun.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\soundent.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\squadmonster.obj" \
	"$(INTDIR)\squeakgrenade.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\talkmonster.obj" \
	"$(INTDIR)\tempmonster.obj" \
	"$(INTDIR)\tentacle.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\tripmine.obj" \
	"$(INTDIR)\turret.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\xen.obj" \
	"$(INTDIR)\zombie.obj"

"$(OUTDIR)\hl.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "hl - Win32 Release"
# Name "hl - Win32 Debug"

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\activity.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\activitymap.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aflock.cpp
DEP_CPP_AFLOC=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_AFLOC=\
	".\platform.h"\
	

"$(INTDIR)\aflock.obj" : $(SOURCE) $(DEP_CPP_AFLOC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\agrunt.cpp
DEP_CPP_AGRUN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\hornet.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\agrunt.obj" : $(SOURCE) $(DEP_CPP_AGRUN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\airtank.cpp
DEP_CPP_AIRTA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_AIRTA=\
	".\platform.h"\
	

"$(INTDIR)\airtank.obj" : $(SOURCE) $(DEP_CPP_AIRTA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animating.cpp
DEP_CPP_ANIMA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_ANIMA=\
	".\platform.h"\
	

"$(INTDIR)\animating.obj" : $(SOURCE) $(DEP_CPP_ANIMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animation.cpp
DEP_CPP_ANIMAT=\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	"..\engine\studio.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\mathlib.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\activitymap.h"\
	".\animation.h"\
	".\enginecallback.h"\
	".\monsterevent.h"\
	".\scriptevent.h"\
	
NODEP_CPP_ANIMAT=\
	".\platform.h"\
	

"$(INTDIR)\animation.obj" : $(SOURCE) $(DEP_CPP_ANIMAT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animation.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\apache.cpp
DEP_CPP_APACH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_APACH=\
	".\platform.h"\
	

"$(INTDIR)\apache.obj" : $(SOURCE) $(DEP_CPP_APACH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\barnacle.cpp
DEP_CPP_BARNA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\barnacle.obj" : $(SOURCE) $(DEP_CPP_BARNA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\barney.cpp
DEP_CPP_BARNE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\talkmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\barney.obj" : $(SOURCE) $(DEP_CPP_BARNE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\basemonster.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bigmomma.cpp
DEP_CPP_BIGMO=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_BIGMO=\
	".\platform.h"\
	

"$(INTDIR)\bigmomma.obj" : $(SOURCE) $(DEP_CPP_BIGMO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bloater.cpp
DEP_CPP_BLOAT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_BLOAT=\
	".\platform.h"\
	

"$(INTDIR)\bloater.obj" : $(SOURCE) $(DEP_CPP_BLOAT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bmodels.cpp
DEP_CPP_BMODE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\doors.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\bmodels.obj" : $(SOURCE) $(DEP_CPP_BMODE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bullsquid.cpp
DEP_CPP_BULLS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_BULLS=\
	".\platform.h"\
	

"$(INTDIR)\bullsquid.obj" : $(SOURCE) $(DEP_CPP_BULLS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\buttons.cpp
DEP_CPP_BUTTO=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\doors.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_BUTTO=\
	".\platform.h"\
	

"$(INTDIR)\buttons.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cbase.cpp
DEP_CPP_CBASE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\client.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\cbase.obj" : $(SOURCE) $(DEP_CPP_CBASE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cbase.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cdll_dll.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\combat.cpp
DEP_CPP_COMBA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\func_break.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\combat.obj" : $(SOURCE) $(DEP_CPP_COMBA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\controller.cpp
DEP_CPP_CONTR=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_CONTR=\
	".\platform.h"\
	

"$(INTDIR)\controller.obj" : $(SOURCE) $(DEP_CPP_CONTR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\crossbow.cpp
DEP_CPP_CROSS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_CROSS=\
	".\platform.h"\
	

"$(INTDIR)\crossbow.obj" : $(SOURCE) $(DEP_CPP_CROSS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\crowbar.cpp
DEP_CPP_CROWB=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_CROWB=\
	".\platform.h"\
	

"$(INTDIR)\crowbar.obj" : $(SOURCE) $(DEP_CPP_CROWB) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\decals.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\defaultai.cpp
DEP_CPP_DEFAU=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_DEFAU=\
	".\platform.h"\
	

"$(INTDIR)\defaultai.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\defaultai.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\doors.cpp
DEP_CPP_DOORS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\doors.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_DOORS=\
	".\platform.h"\
	

"$(INTDIR)\doors.obj" : $(SOURCE) $(DEP_CPP_DOORS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\doors.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\effects.cpp
DEP_CPP_EFFEC=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\customentity.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\../engine\shake.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\func_break.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_EFFEC=\
	".\platform.h"\
	

"$(INTDIR)\effects.obj" : $(SOURCE) $(DEP_CPP_EFFEC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\effects.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\egon.cpp
DEP_CPP_EGON_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\customentity.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_EGON_=\
	".\platform.h"\
	

"$(INTDIR)\egon.obj" : $(SOURCE) $(DEP_CPP_EGON_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\enginecallback.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\explode.cpp
DEP_CPP_EXPLO=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\explode.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\explode.obj" : $(SOURCE) $(DEP_CPP_EXPLO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\explode.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\extdll.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\flyingmonster.cpp
DEP_CPP_FLYIN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\flyingmonster.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_FLYIN=\
	".\platform.h"\
	

"$(INTDIR)\flyingmonster.obj" : $(SOURCE) $(DEP_CPP_FLYIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\flyingmonster.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\func_break.cpp
DEP_CPP_FUNC_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\explode.h"\
	".\extdll.h"\
	".\func_break.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_FUNC_=\
	".\platform.h"\
	

"$(INTDIR)\func_break.obj" : $(SOURCE) $(DEP_CPP_FUNC_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\func_break.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\func_tank.cpp
DEP_CPP_FUNC_T=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\explode.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_FUNC_T=\
	".\platform.h"\
	

"$(INTDIR)\func_tank.obj" : $(SOURCE) $(DEP_CPP_FUNC_T) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gamerules.cpp
DEP_CPP_GAMER=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_GAMER=\
	".\platform.h"\
	

"$(INTDIR)\gamerules.obj" : $(SOURCE) $(DEP_CPP_GAMER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gamerules.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gargantua.cpp
DEP_CPP_GARGA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\customentity.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\explode.h"\
	".\extdll.h"\
	".\func_break.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_GARGA=\
	".\platform.h"\
	

"$(INTDIR)\gargantua.obj" : $(SOURCE) $(DEP_CPP_GARGA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gauss.cpp
DEP_CPP_GAUSS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\../engine\shake.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_GAUSS=\
	".\platform.h"\
	

"$(INTDIR)\gauss.obj" : $(SOURCE) $(DEP_CPP_GAUSS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\genericmonster.cpp
DEP_CPP_GENER=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_GENER=\
	".\platform.h"\
	

"$(INTDIR)\genericmonster.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ggrenade.cpp
DEP_CPP_GGREN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\ggrenade.obj" : $(SOURCE) $(DEP_CPP_GGREN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\globals.cpp
DEP_CPP_GLOBA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_GLOBA=\
	".\platform.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\glock.cpp
DEP_CPP_GLOCK=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_GLOCK=\
	".\platform.h"\
	

"$(INTDIR)\glock.obj" : $(SOURCE) $(DEP_CPP_GLOCK) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gman.cpp
DEP_CPP_GMAN_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_GMAN_=\
	".\platform.h"\
	

"$(INTDIR)\gman.obj" : $(SOURCE) $(DEP_CPP_GMAN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\h_ai.cpp
DEP_CPP_H_AI_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_H_AI_=\
	".\platform.h"\
	

"$(INTDIR)\h_ai.obj" : $(SOURCE) $(DEP_CPP_H_AI_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\h_battery.cpp
DEP_CPP_H_BAT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_H_BAT=\
	".\platform.h"\
	

"$(INTDIR)\h_battery.obj" : $(SOURCE) $(DEP_CPP_H_BAT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\h_cine.cpp
DEP_CPP_H_CIN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_H_CIN=\
	".\platform.h"\
	

"$(INTDIR)\h_cine.obj" : $(SOURCE) $(DEP_CPP_H_CIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\h_cycler.cpp
DEP_CPP_H_CYC=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_H_CYC=\
	".\platform.h"\
	

"$(INTDIR)\h_cycler.obj" : $(SOURCE) $(DEP_CPP_H_CYC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\h_export.cpp
DEP_CPP_H_EXP=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_H_EXP=\
	".\platform.h"\
	

"$(INTDIR)\h_export.obj" : $(SOURCE) $(DEP_CPP_H_EXP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\handgrenade.cpp
DEP_CPP_HANDG=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HANDG=\
	".\platform.h"\
	

"$(INTDIR)\handgrenade.obj" : $(SOURCE) $(DEP_CPP_HANDG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hassassin.cpp
DEP_CPP_HASSA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HASSA=\
	".\platform.h"\
	

"$(INTDIR)\hassassin.obj" : $(SOURCE) $(DEP_CPP_HASSA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\headcrab.cpp
DEP_CPP_HEADC=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\headcrab.obj" : $(SOURCE) $(DEP_CPP_HEADC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\healthkit.cpp
DEP_CPP_HEALT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\items.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HEALT=\
	".\platform.h"\
	

"$(INTDIR)\healthkit.obj" : $(SOURCE) $(DEP_CPP_HEALT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hgrunt.cpp
DEP_CPP_HGRUN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\customentity.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\plane.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\talkmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HGRUN=\
	".\platform.h"\
	

"$(INTDIR)\hgrunt.obj" : $(SOURCE) $(DEP_CPP_HGRUN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hornet.cpp
DEP_CPP_HORNE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\hornet.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HORNE=\
	".\platform.h"\
	

"$(INTDIR)\hornet.obj" : $(SOURCE) $(DEP_CPP_HORNE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hornet.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hornetgun.cpp
DEP_CPP_HORNET=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\hornet.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_HORNET=\
	".\platform.h"\
	

"$(INTDIR)\hornetgun.obj" : $(SOURCE) $(DEP_CPP_HORNET) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\houndeye.cpp
DEP_CPP_HOUND=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_HOUND=\
	".\platform.h"\
	

"$(INTDIR)\houndeye.obj" : $(SOURCE) $(DEP_CPP_HOUND) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ichthyosaur.cpp
DEP_CPP_ICHTH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\flyingmonster.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_ICHTH=\
	".\platform.h"\
	

"$(INTDIR)\ichthyosaur.obj" : $(SOURCE) $(DEP_CPP_ICHTH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\islave.cpp
DEP_CPP_ISLAV=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_ISLAV=\
	".\platform.h"\
	

"$(INTDIR)\islave.obj" : $(SOURCE) $(DEP_CPP_ISLAV) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\items.cpp
DEP_CPP_ITEMS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\items.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_ITEMS=\
	".\platform.h"\
	

"$(INTDIR)\items.obj" : $(SOURCE) $(DEP_CPP_ITEMS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\items.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\leech.cpp
DEP_CPP_LEECH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_LEECH=\
	".\platform.h"\
	

"$(INTDIR)\leech.obj" : $(SOURCE) $(DEP_CPP_LEECH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lights.cpp
DEP_CPP_LIGHT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_LIGHT=\
	".\platform.h"\
	

"$(INTDIR)\lights.obj" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\monsterevent.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\monstermaker.cpp
DEP_CPP_MONST=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_MONST=\
	".\platform.h"\
	

"$(INTDIR)\monstermaker.obj" : $(SOURCE) $(DEP_CPP_MONST) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\monsters.cpp
DEP_CPP_MONSTE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_MONSTE=\
	".\platform.h"\
	

"$(INTDIR)\monsters.obj" : $(SOURCE) $(DEP_CPP_MONSTE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\monsters.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\monsterstate.cpp
DEP_CPP_MONSTER=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_MONSTER=\
	".\platform.h"\
	

"$(INTDIR)\monsterstate.obj" : $(SOURCE) $(DEP_CPP_MONSTER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mortar.cpp
DEP_CPP_MORTA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_MORTA=\
	".\platform.h"\
	

"$(INTDIR)\mortar.obj" : $(SOURCE) $(DEP_CPP_MORTA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mp5.cpp
DEP_CPP_MP5_C=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_MP5_C=\
	".\platform.h"\
	

"$(INTDIR)\mp5.obj" : $(SOURCE) $(DEP_CPP_MP5_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multiplay_gamerules.cpp
DEP_CPP_MULTI=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\items.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_MULTI=\
	".\platform.h"\
	

"$(INTDIR)\multiplay_gamerules.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\nihilanth.cpp
DEP_CPP_NIHIL=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_NIHIL=\
	".\platform.h"\
	

"$(INTDIR)\nihilanth.obj" : $(SOURCE) $(DEP_CPP_NIHIL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\nodes.cpp
DEP_CPP_NODES=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\doors.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_NODES=\
	".\platform.h"\
	

"$(INTDIR)\nodes.obj" : $(SOURCE) $(DEP_CPP_NODES) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\nodes.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\osprey.cpp
DEP_CPP_OSPRE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\customentity.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_OSPRE=\
	".\platform.h"\
	

"$(INTDIR)\osprey.obj" : $(SOURCE) $(DEP_CPP_OSPRE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\pathcorner.cpp
DEP_CPP_PATHC=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\trains.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_PATHC=\
	".\platform.h"\
	

"$(INTDIR)\pathcorner.obj" : $(SOURCE) $(DEP_CPP_PATHC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\plane.cpp
DEP_CPP_PLANE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\extdll.h"\
	".\plane.h"\
	".\vector.h"\
	
NODEP_CPP_PLANE=\
	".\platform.h"\
	

"$(INTDIR)\plane.obj" : $(SOURCE) $(DEP_CPP_PLANE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\plane.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\plats.cpp
DEP_CPP_PLATS=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\trains.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_PLATS=\
	".\platform.h"\
	

"$(INTDIR)\plats.obj" : $(SOURCE) $(DEP_CPP_PLATS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\player.cpp
DEP_CPP_PLAYE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\../engine\shake.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\trains.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\player.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\python.cpp
DEP_CPP_PYTHO=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_PYTHO=\
	".\platform.h"\
	

"$(INTDIR)\python.obj" : $(SOURCE) $(DEP_CPP_PYTHO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rat.cpp
DEP_CPP_RAT_C=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_RAT_C=\
	".\platform.h"\
	

"$(INTDIR)\rat.obj" : $(SOURCE) $(DEP_CPP_RAT_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\roach.cpp
DEP_CPP_ROACH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\roach.obj" : $(SOURCE) $(DEP_CPP_ROACH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rpg.cpp
DEP_CPP_RPG_C=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_RPG_C=\
	".\platform.h"\
	

"$(INTDIR)\rpg.obj" : $(SOURCE) $(DEP_CPP_RPG_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\satchel.cpp
DEP_CPP_SATCH=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_SATCH=\
	".\platform.h"\
	

"$(INTDIR)\satchel.obj" : $(SOURCE) $(DEP_CPP_SATCH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\saverestore.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\schedule.cpp
DEP_CPP_SCHED=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SCHED=\
	".\platform.h"\
	

"$(INTDIR)\schedule.obj" : $(SOURCE) $(DEP_CPP_SCHED) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\schedule.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scientist.cpp
DEP_CPP_SCIEN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\talkmonster.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SCIEN=\
	".\platform.h"\
	

"$(INTDIR)\scientist.obj" : $(SOURCE) $(DEP_CPP_SCIEN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\scripted.cpp
DEP_CPP_SCRIP=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SCRIP=\
	".\platform.h"\
	

"$(INTDIR)\scripted.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\scripted.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scriptevent.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\shotgun.cpp
DEP_CPP_SHOTG=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_SHOTG=\
	".\platform.h"\
	

"$(INTDIR)\shotgun.obj" : $(SOURCE) $(DEP_CPP_SHOTG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\singleplay_gamerules.cpp
DEP_CPP_SINGL=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\items.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_SINGL=\
	".\platform.h"\
	

"$(INTDIR)\singleplay_gamerules.obj" : $(SOURCE) $(DEP_CPP_SINGL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\skill.cpp
DEP_CPP_SKILL=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SKILL=\
	".\platform.h"\
	

"$(INTDIR)\skill.obj" : $(SOURCE) $(DEP_CPP_SKILL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\skill.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.cpp
DEP_CPP_SOUND=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\talkmonster.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\soundent.cpp
DEP_CPP_SOUNDE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SOUNDE=\
	".\platform.h"\
	

"$(INTDIR)\soundent.obj" : $(SOURCE) $(DEP_CPP_SOUNDE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\soundent.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spectator.cpp
DEP_CPP_SPECT=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\spectator.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SPECT=\
	".\platform.h"\
	

"$(INTDIR)\spectator.obj" : $(SOURCE) $(DEP_CPP_SPECT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\spectator.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\squadmonster.cpp
DEP_CPP_SQUAD=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\plane.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\squadmonster.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SQUAD=\
	".\platform.h"\
	

"$(INTDIR)\squadmonster.obj" : $(SOURCE) $(DEP_CPP_SQUAD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\squadmonster.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\squeakgrenade.cpp
DEP_CPP_SQUEA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_SQUEA=\
	".\platform.h"\
	

"$(INTDIR)\squeakgrenade.obj" : $(SOURCE) $(DEP_CPP_SQUEA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\subs.cpp
DEP_CPP_SUBS_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\doors.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\nodes.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_SUBS_=\
	".\platform.h"\
	

"$(INTDIR)\subs.obj" : $(SOURCE) $(DEP_CPP_SUBS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\talkmonster.cpp
DEP_CPP_TALKM=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\defaultai.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\scripted.h"\
	".\scriptevent.h"\
	".\skill.h"\
	".\soundent.h"\
	".\talkmonster.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_TALKM=\
	".\platform.h"\
	

"$(INTDIR)\talkmonster.obj" : $(SOURCE) $(DEP_CPP_TALKM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\talkmonster.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tempmonster.cpp
DEP_CPP_TEMPM=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_TEMPM=\
	".\platform.h"\
	

"$(INTDIR)\tempmonster.obj" : $(SOURCE) $(DEP_CPP_TEMPM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tentacle.cpp
DEP_CPP_TENTA=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	

"$(INTDIR)\tentacle.obj" : $(SOURCE) $(DEP_CPP_TENTA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\trains.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\triggers.cpp
DEP_CPP_TRIGG=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\trains.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\triggers.obj" : $(SOURCE) $(DEP_CPP_TRIGG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tripmine.cpp
DEP_CPP_TRIPM=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_TRIPM=\
	".\platform.h"\
	

"$(INTDIR)\tripmine.obj" : $(SOURCE) $(DEP_CPP_TRIPM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\turret.cpp
DEP_CPP_TURRE=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_TURRE=\
	".\platform.h"\
	

"$(INTDIR)\turret.obj" : $(SOURCE) $(DEP_CPP_TURRE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp
DEP_CPP_UTIL_=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\../engine\shake.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_UTIL_=\
	".\platform.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vector.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\weapons.cpp
DEP_CPP_WEAPO=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_WEAPO=\
	".\platform.h"\
	

"$(INTDIR)\weapons.obj" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\weapons.h

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\world.cpp
DEP_CPP_WORLD=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\decals.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\nodes.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\soundent.h"\
	".\util.h"\
	".\vector.h"\
	".\weapons.h"\
	
NODEP_CPP_WORLD=\
	".\platform.h"\
	

"$(INTDIR)\world.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\xen.cpp
DEP_CPP_XEN_C=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\animation.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\effects.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_XEN_C=\
	".\platform.h"\
	

"$(INTDIR)\xen.obj" : $(SOURCE) $(DEP_CPP_XEN_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zombie.cpp
DEP_CPP_ZOMBI=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\monsterevent.h"\
	".\monsters.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\skill.h"\
	".\util.h"\
	".\vector.h"\
	
NODEP_CPP_ZOMBI=\
	".\platform.h"\
	

"$(INTDIR)\zombie.obj" : $(SOURCE) $(DEP_CPP_ZOMBI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hl.def

!IF  "$(CFG)" == "hl - Win32 Release"

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\client.cpp
DEP_CPP_CLIEN=\
	"..\dlls\cdll_dll.h"\
	"..\engine\custom.h"\
	"..\engine\cvardef.h"\
	".\../common\platform.h"\
	".\../engine\const.h"\
	".\../engine\eiface.h"\
	".\../engine\progdefs.h"\
	".\../engine\progs.h"\
	".\activity.h"\
	".\basemonster.h"\
	".\cbase.h"\
	".\client.h"\
	".\enginecallback.h"\
	".\extdll.h"\
	".\gamerules.h"\
	".\monsterevent.h"\
	".\player.h"\
	".\saverestore.h"\
	".\schedule.h"\
	".\soundent.h"\
	".\spectator.h"\
	".\util.h"\
	".\vector.h"\
	

"$(INTDIR)\client.obj" : $(SOURCE) $(DEP_CPP_CLIEN) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
