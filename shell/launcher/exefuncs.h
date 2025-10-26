// exefuncs.h
#ifndef EXEFUNCS_H
#define EXEFUNCS_H

// Engine hands this to DLLs for functionality callbacks
typedef struct exefuncs_s
{
	int			fMMX;
	int			iCPUMhz;

	// Functions
	void		(*VID_LockBuffer)( void );
	void		(*VID_UnlockBuffer)( void );
	// Called at shutdown
	void		(*VID_Shutdown)( void );
	// Flushes the given rectangles from the view buffer to the screen
	void		(*VID_Update)( vrect_t* rects );
	int			(*VID_ForceLockState)( int lk );
	int			(*VID_ForceUnlockedAndReturnState)( void );
	void		(*VID_SetDefaultMode)( void );
	// Gets the mode driver name in description for console
	char*		(*VID_GetExtModeDescription)( int mode );
	// Get viddef structure
	void		(*VID_GetVID)( viddef_t* pvid );

	void		(*D_BeginDirectRect)( int width, int height, byte* data, int pic_width, int pic_height );
	void		(*D_EndDirectRect)( int width, int height, int pic_width, int pic_height );

	// Let the launcher know when the app is being activated or deactivated
	void		(*AppActivate)( int fActive, int minimize );

	// CD Audio
	void		(*CDAudio_Play)( int track, int looping );
	void		(*CDAudio_Pause)( void );
	void		(*CDAudio_Resume)( void );
	void		(*CDAudio_Update)( void );

	// Initializes launcher commands
	void		(*Launcher_InitCmds)( void );

	// Print/error callbacks
	void        (*ErrorMessage)( int nLevel, const char* pszErrorMessage );

	int			(*D_SurfaceCacheForRes)( int width, int height );

	void        (*Console_Printf)( char* fmt, ... );

	// Retrieves the user's raw cd key
	void		(*Launcher_GetCDKey)( char* pszCDKey, int* nLength, int* bDedicated );
	void		(*Launcher_GetClientID)( void* pID );
	void		(*Launcher_GetUUID)( void* pUUID, int* nLength, int* bDedicated );
	void*		(*Launcher_VerifyMessage)( int nLength, byte* pKey, int nMsgLength, char* pMsg, int nSignLength, byte* pSign );
	// Retrieves the user's raw auth certificate
	int			(*Launcher_GetCertificate)( void* pBuffer, int* nLength );
	int			(*Launcher_RequestNewClientCertificate)( void );
	int			(*Launcher_ValidateClientCertificate)( void* pBuffer, int nLength );
} exefuncs_t;

typedef void(*xcommand_t) ( void );

// should be in exefuncs.h
void	VID_LockBuffer( void );
void	VID_UnlockBuffer( void );
void	VID_Shutdown( void );
void	VID_Update( struct vrect_s* rects );
int		VID_ForceUnlockedAndReturnState( void );
void	VID_SetDefaultMode( void );
char*	VID_GetExtModeDescription( int mode );
int		VID_ForceLockState( int lk );
void	VID_GetVID( struct viddef_s* pvid );
int		D_SurfaceCacheForRes( int width, int height );
void	D_BeginDirectRect( int width, int height, byte* data, int pic_width, int pic_height );
void	D_EndDirectRect( int width, int height, int pic_width, int pic_height );
void	AppActivate( int fActive, int minimize );
void	CDAudio_Play( int track, int looping );
void	CDAudio_Pause( void );
void	CDAudio_Resume( void );
void	CDAudio_Update( void );
void	ErrorMessage( int nLevel, const char* pszErrorMessage );
void	Console_Printf( char* fmt, ... );
void	Launcher_InitCmds( void );
void	Launcher_GetCDKey( char* pszCDKey, int* nLength, int* bDedicated );
void	Launcher_GetClientID( void* pID );
void	Launcher_GetUUID( void* pUUID, int* nLength, int* bDedicated );
void*	Launcher_VerifyMessage( int nLength, byte* pKey, int nMsgLength, char* pMsg, int nSignLength, byte* pSign );
int		Launcher_GetCertificate( void* pBuffer, int* nLength );
int		Launcher_RequestNewClientCertificate( void );
int		Launcher_ValidateClientCertificate( void* pBuffer, int nLength );

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
extern int   (*GetGameInfo)( struct GameInfo_s* pGI, char* pszChannel );
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

#endif