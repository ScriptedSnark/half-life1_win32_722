// comndef.h  -- general definitions

typedef int qboolean;







//============================================================================



int Q_strlen( char* str );


int Q_strcmp( char* s1, char* s2 );

int	Q_atoi( char* str );
float Q_atof( char* str );

//============================================================================




extern	int		com_argc;
extern	char** com_argv;

int COM_CheckParm( char* parm );
void COM_InitArgv( int argc, char** argv );





