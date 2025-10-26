//=============================================
// HALF-LIFE NET TEST 1 LAUNCHER REIMPLEMENTATION
// No GUI version
//
// Author: ScriptedSnark
// File: launcher.h
//=============================================

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "platform.h"
#include "sys.h"
#include "vid.h"
#include "dll_state.h"

typedef enum
{
	ca_dedicated = 0,	// This is a dedicated server, client code is inactive
	ca_disconnected,	// full screen console with no connection
	ca_connecting,		// netchan_t established, waiting for svc_serverdata
	ca_connected,		// processing data lists, donwloading, etc
	ca_uninitialized,	// valid netcon, autodownloading
	ca_active			// everything is in, so frames can be rendered
} cactive_t;


#include "gameinfo.h"
#include <SDL2/SDL.h>
#include "exefuncs.h"
#include "util.h"


#ifdef __cplusplus
#define DECLTYPE(func) (decltype(func))
#else
#define DECLTYPE(func) (void*)
#endif


extern SDL_Window* gWindow;
extern SDL_GLContext gGLContext;

extern exefuncs_t ef;
extern VidTypes g_vidType;
extern char sys_cmdline[2048];

void Eng_SetGameState(int state);
void Eng_SetSubGameState(int state);
void Eng_Frame(qboolean forceUpdate);
qboolean Eng_Load(const char* szDllName, int subState);
void Eng_Unload(void);

extern void  (*Cbuf_AddText)( char* text );
extern void  (*Cbuf_InsertText)( char* text );
extern void  (*Cmd_AddCommand)( char* cmd_name, xcommand_t function );
extern int   (*Cmd_Argc)( void );
extern char* (*Cmd_Argv)( int arg );
extern char* (*Cmd_Args)( void );
extern int   (*COM_CheckParmEx)( char* parm, char** argv );
extern void  (*DeinitConProc)( void );
extern int   (*Con_GetInput)( char* text, int linepos );
extern void  (*Con_Printf)( char* fmt, ... );
extern void  (*Con_DPrintf)( char* fmt, ... );
extern void  (*Con_SafePrintf)( char* fmt, ... );
extern void  (*Cvar_Set)( char* var_name, char* value );
extern void  (*Cvar_SetValue)( char* var_name, float value );
extern float (*Cvar_VariableValue)( char* var_name );
extern int   (*Cvar_VariableInt)( char* var_name );
extern char* (*Cvar_VariableString)( char* var_name );
extern char* (*Cvar_CompleteVariable)( char* partial );
extern int   (*GL_SetMode)( SDL_Window* mainwindow );
extern void  (*GL_Shutdown)( SDL_Window* window );
extern void  (*VID_UpdateWindowVars)( void* prc, int x, int y );
extern void  (*VID_UpdateVID)( viddef_t* pvid );
extern int   (*VID_AllocBuffers)( void );
extern void  (*Host_GetHostInfo)( float* fps, int* nActive, int* nBots, int* nMaxPlayers, char* pszMap );
extern int   (*Host_Frame)( float time, int iState, int* stateInfo );
extern BOOL  (*SaveGame)( char* pszSlot, char* pszComment );
extern int   (*LoadGame)( char* pName );
extern void  (*IN_ClearStates)( void );
extern void  (*Key_UpLine)( void );
extern void  (*Key_Event)( int key, qboolean down );
extern void  (*Key_ClearStates)( void );
extern void  (*Download4444)( void );
extern void  (*QGL_D3DShared)( void* d3dGShared );
extern void  (*GL_Init)( void );
extern BOOL (  *qwglSwapBuffers )( void* );
extern void  (*Host_Shutdown)( void );
extern void  (*S_ClearBuffer)( void );
extern void  (*SNDDMA_Shutdown)( void );
extern void  (*Snd_ReleaseBuffer)( void );
extern void  (*Snd_AcquireBuffer)( void );
extern double (*Sys_FloatTime)( void );
extern void  (*S_GetDSPointer)( void* lpDS, void* lpDSBuf );
extern void* (*S_GetWAVPointer)( void );
extern double (*Sys_FloatTime)( void );
extern int   (*GetEngineState)( void );
extern void  (*GameSetSubState)( int iSubState );
extern void  (*GameSetState)( int iState );
extern void  (*GameSetBackground)( qboolean bNewSetting );
extern int   (*GameInit)( char* lpCmdLine, unsigned char* pMem, int iSize, exefuncs_t* pef, void* pwnd, char* profile, int bIsDedicated, char* szBaseDir );
extern int   (*GetGameInfo)( GameInfo_t* pGI, char* pszChannel );
extern void  (*ForceReloadProfile)( char* pszName );
extern void  (*SetStartupMode)( qboolean bMode );
extern void  (*SetMessagePumpDisableMode)( qboolean bMode );
extern void  (*SetPauseState)( qboolean bPause );
extern int   (*GetPauseState)( void );
extern void  (*Keyboard_ReturnToGame)( void );
extern void  (*IN_ShowMouse)( void );
extern void  (*IN_DeactivateMouse)( void );
extern void  (*IN_HideMouse)( void );
extern void  (*IN_ActivateMouse)( void );
extern void  (*IN_MouseEvent)( int mstate );
extern void  (*IN_UpdateClipCursor)( void );
extern void  (*S_BlockSound)( void );
extern void  (*S_UnblockSound)( void );

#endif // LAUNCHER_H