//=============================================
// HALF-LIFE NET TEST 1 LAUNCHER REIMPLEMENTATION
// No GUI version
//
// Author: ScriptedSnark
// File: launcher.h
//=============================================

#include "platform.h"
#include "sys.h"
#include "exefuncs.h"
#include "dll_state.h"
#include "vid.h"


#ifdef __cplusplus
#define DECLTYPE(func) (decltype(func))
#else
#define DECLTYPE(func) (void*)
#endif

extern exefuncs_t ef;
extern VidTypes g_vidType;

extern HWND hWnd;
extern HDC hDC;
extern HGLRC hRC;

void Eng_SetGameState(int state);
void Eng_SetSubGameState(int state);
void Eng_Frame(qboolean forceUpdate);
qboolean Eng_Load(const char* szDllName, int subState);
void Eng_Unload(void);

void Launcher_CenterWindow(HWND hwnd);
