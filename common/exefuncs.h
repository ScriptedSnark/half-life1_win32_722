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
	void		(*VID_Shutdown)( void );
	void		(*VID_Update)( struct vrect_s* rects );
	void		(*VID_ForceLockState)( int lk );
	int			(*VID_ForceUnlockedAndReturnState)( void );
	void		(*VID_SetDefaultMode)( void );
	char*		(*VID_GetExtModeDescription)( int mode );
	// Get viddef structure
	void		(*VID_GetVID)( struct viddef_s* pvid );

	void		(*D_BeginDirectRect)( int width, int height, byte* data, int pic_width, int pic_height );
	void		(*D_EndDirectRect)( int width, int height, int pic_width, int pic_height );

	void		(*AppActivate)( int fActive, int minimize );

	void		(*CDAudio_Play)( int track, int looping );
	void		(*CDAudio_Pause)( void );
	void		(*CDAudio_Resume)( void );
	void		(*CDAudio_Update)( void );

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
	int			(*Launcher_GetCertificate)( void* pBuffer, int* nLength );
	int			(*Launcher_RequestNewClientCertificate)( void );
	int			(*Launcher_ValidateClientCertificate)( void* pBuffer, int nLength );
} exefuncs_t;

#endif