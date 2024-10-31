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

void Sys_Warning( char* warning, ... );

void Sys_Printf( char* fmt, ... );
// send text to the console

void Sys_Quit( void );

double Sys_FloatTime( void );