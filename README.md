# half-life1_win32_722 <img src="https://github.com/user-attachments/assets/6c8584a8-e619-4038-b432-6299394c61af" width="120" align="right">

Reverse-engineered source code of Half-Life 1 Net Test 1 (engine build 722), an early pre-release version of the GoldSrc engine (known at the time as "The Half-Life Engine") distributed to playtesters.

The build was released in October 2024 by playtester Chad Jessup and later reverse-engineered by our team.

<img width="2557" height="1391" alt="image_2025-10-06_00-14-41" src="https://github.com/user-attachments/assets/51612c59-521b-41a3-a452-07fa1359db5d" />

---

## 🧩 Getting Started

Before building the project, you need to configure the deployment path.

The file **DeployPath.ini** defines where compiled binaries will be automatically deployed.  
Example:
```
; Working directory
root.path="E:\Half-Life NetTest1"
```

## 🛠️ Building the Project
To compile the project, you will need Microsoft Visual Studio 2022.
Open the solution file located at:
```
...\half-life1_win32_722\projects\vs2022\Half-Life (722 Build, 1998).sln
```
Don't forget to set the Working Directory in the project properties, otherwise the engine may fail to load the GameDLL.

## 📂 Required Files
You will need the original data files from Half-Life 1 Net Test 1.
They are publicly available online and can be found on archival sources such as the Internet Archive.

## 👥 People involved in development of the project:
- [\*(int\*)0 = 0xDEADBEEF](https://github.com/KV-Stepchenko)
- [ScriptedSnark](https://github.com/ScriptedSnark)
- [xWhitey](https://github.com/autisoid)
- [s1lentq](https://github.com/s1lentq)
- [oxiKKK](https://github.com/oxiKKK)

## 📚 Referenced projects:
- [WinQuake / QuakeWorld](https://github.com/id-Software/Quake)
- [QGL Wrapper (Quake II)](https://github.com/id-Software/Quake-2/blob/master/win32/qgl_win.c)
- [Half-Life SDK 1.0](https://github.com/ScriptedSnark/hlsdk-versions/tree/hlsdk_sp_1_0)
- [Half-Life SDK 2.3](https://github.com/ValveSoftware/halflife)