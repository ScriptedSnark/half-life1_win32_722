// sys.h -- non-portable functions

//
// file IO
//

// returns the file size
// return -1 if file is not present
// the file should be in BINARY mode for stupid OSs that care
int Sys_FileOpenRead( char* path, int* hndl );

int Sys_FileOpenWrite( char* path );
void Sys_FileClose( int handle );
void Sys_FileSeek( int handle, int position );
int Sys_FileRead( int handle, void* dest, int count );
int Sys_FileWrite( int handle, void* data, int count );
int	Sys_FileTime( char* path );
void Sys_mkdir( char* path );
int Sys_FileTell( int i );

//
// memory protection
//
void Sys_MakeCodeWriteable( unsigned long startaddr, unsigned long length );





void Sys_Error( char* error, ... );
// an error will cause the entire program to exit

void Sys_Warning( char* fmt, ... );

void Sys_Printf( char* fmt, ... );
// send text to the console

void Sys_Quit( void );

DLL_EXPORT double Sys_FloatTime( void );

void Sys_Sleep( void );
// called to yield for a little bit so as
// not to hog cpu when paused or debugging

void Sys_SendKeyEvents( void );
// Perform Key_Event () callbacks until the input que is empty

// Loads all entity DLLs
// szBaseDir - Base directory to look for entity dlls in if no liblist could be loaded
void LoadEntityDLLs( const char* szBaseDir );

// Loads Server DLL
// szDllFilename - File name of dll to load
void LoadThisDll( char* szDllFilename );

// Releases all entity dlls
void ReleaseEntityDlls( void );

void Sys_LowFPPrecision( void );
void Sys_HighFPPrecision( void );
void Sys_SetFPCW( void );