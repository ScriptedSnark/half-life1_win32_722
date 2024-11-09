// comndef.h  -- general definitions

typedef int qboolean;







//============================================================================

void Q_memset( void* dest, int fill, int count );
void Q_memcpy( void* dest, void* src, int count );




void Q_strcpy( char* dest, char* src );
void Q_strncpy( char* dest, char* src, int count );
int Q_strlen( char* str );


int Q_strcmp( char* s1, char* s2 );
int Q_strncmp( char* s1, char* s2, int count );
int Q_strcasecmp( char* s1, char* s2 );
int Q_strncasecmp( char* s1, char* s2, int n );
int	Q_atoi( char* str );
float Q_atof( char* str );

//============================================================================

extern	char		com_token[1024];
extern qboolean com_ignorecolons;

char* COM_Parse( char* data );


extern	int		com_argc;
extern	char** com_argv;

int COM_CheckParm( char* parm );
void COM_InitArgv( int argc, char** argv );





