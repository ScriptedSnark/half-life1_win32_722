// refresh.h -- public interface to refresh functions

// TODO: Implement

typedef struct
{
	vec3_t vieworg;
} refdef_t;

// TODO: Implement

extern	refdef_t	r_refdef;


void R_Init( void );
void R_InitTextures( void );


void R_SetStackBase( void );

// TODO: Implement