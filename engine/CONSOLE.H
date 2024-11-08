//
// console
//
extern int con_totallines;
extern int con_backscroll;
extern int con_rows;
extern	qboolean con_forcedup;	// because no entities to refresh

DLL_EXPORT void Con_Printf( char* fmt, ... );
DLL_EXPORT void Con_DPrintf( char* fmt, ... );



void Con_ToggleConsole_f( void );