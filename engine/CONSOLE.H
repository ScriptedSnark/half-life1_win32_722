//
// console
//
extern qboolean con_loading;
extern int con_totallines;
extern int con_backscroll;
extern int con_rows;
extern qboolean con_forcedup;	// because no entities to refresh
extern qboolean con_initialized;
extern int	con_notifylines;		// scan lines to clear for notify lines

void Con_CheckResize( void );
void Con_Init( void );
void Con_Shutdown( void );   // Free overlay line buffer.
void Con_DrawConsole( int lines, qboolean drawinput );
void Con_Print( char* txt );
DLL_EXPORT void Con_Printf( char* fmt, ... );
DLL_EXPORT void Con_DPrintf( char* fmt, ... );
DLL_EXPORT void Con_SafePrintf( char* fmt, ... );
void Con_Clear_f( void );
void Con_DrawNotify( void );
void Con_ClearNotify( void );
void Con_ToggleConsole_f( void );

void Con_NotifyBox( char* text );	// during startup for sound / cd warnings