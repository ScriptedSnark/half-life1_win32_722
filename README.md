# half-life1_win32_722 <img src="media/logo.png" width="120" align="right">

Reverse-engineered source code of Half-Life 1 Net Test 1 (engine build 722), an early pre-release version of the GoldSrc engine (known at the time as "The Half-Life Engine") distributed to playtesters.

The build was published in October 2024 by playtester [Chad Jessup](https://x.com/ubahs1337) and later reverse-engineered by our team.

<img alt="NETTEST1_VS2022" src="media/nettest1_vs2022.png" />

This repository contains:
- Restored source code of HL SDK in Net Test 1 era
	- [x] client.dll
	- [x] hl.dll
- Restored source code of the engine (no A3D impl.)
	- [x] sw.dll
	- [x] hw.dll (with Direct3D support)
- Restored source code of the localization module
	- [x] hl_res.dll
- Reimplemented lightweight source code of the launcher without MFC/SW code
	- [x] hl.exe (see [`linux`](https://github.com/ScriptedSnark/half-life1_win32_722/tree/linux) branch)

---

## Getting started

**Before building the project, you need to configure the deployment path (ONLY FOR VS2022).**

The file **DeployPath.ini** defines where compiled binaries will be automatically deployed.  
Example:
```
; Working directory
root.path="E:\Half-Life NetTest1"
```

## Building

### Visual Studio 2022

Open the solution file located at:
```
..\half-life1_win32_722\projects\vs2022\Half-Life (722 Build, 1998).sln
```

Now press `Build` -> `Build Solution` (or press `Ctrl+Shift+B` hotkey).

**NOTE: if you get afxres.h error during hl_res.dll building, install Microsoft Foundation Class (MFC) library via VS installer.**

### Visual C++ 4.2
Open any specific project you want to build, for example:
```
..\half-life1_win32_722\engine\Quiver.mak
```
All project files are located in their directories.

Now press `Build`  -> `Build X.dll` (or press `Shift+F8` hotkey).

**NOTE: if you get ml.exe errors, install MASM 6.12 or newer right into your MSVC++ 4.2 (ml.exe should be inside `bin` directory or somewhere in PATH environment).**

**WARNING: don't forget to set the Working Directory in the project properties, otherwise the engine may fail to load the GameDLL during debugging.**

## Required files for playing
You will need the original data files from Half-Life 1 Net Test 1.
They are publicly available online and can be found on archival sources such as the Internet Archive.

**For the best NT1 experience, you can also use WON dlls/hl_res/launcher from the [build 738](https://www.betaarchive.com/wiki/index.php/Half-Life/build_738) or later to avoid some window bugs.**

## People involved in development of the project
- [\*(int\*)0 = 0xDEADBEEF](https://github.com/KV-Stepchenko)
- [ScriptedSnark](https://github.com/ScriptedSnark)
- [xWhitey](https://github.com/autisoid)
- [s1lentq](https://github.com/s1lentq)
- [oxiKKK](https://github.com/oxiKKK)

## Referenced projects
- [WinQuake / QuakeWorld](https://github.com/id-Software/Quake)
- [QGL Wrapper (Quake II)](https://github.com/id-Software/Quake-2/blob/master/win32/qgl_win.c)
- [Half-Life SDK](https://github.com/ValveSoftware/halflife)
- [Half-Life SDK 1.0](https://github.com/ScriptedSnark/hlsdk-versions/tree/hlsdk_sp_1_0)
- [Half-Life SDK 2.3](https://github.com/ScriptedSnark/hlsdk-versions/tree/hlsdk_sp_2_3)

## Special thanks
- [Overfloater](https://github.com/TheOverfloater) (for playtesting)
- [Barspinoff](https://github.com/barspinoff) (for playtesting and assistance with software rendering)
- [GoldSrc Underground Discord](https://discord.gg/TwMz3e2sVW)