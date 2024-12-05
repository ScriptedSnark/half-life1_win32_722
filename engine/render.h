// refresh.h -- public interface to refresh functions

// TODO: Implement

typedef struct
{
	vrect_t		vrect;								// subwindow in video for refresh
													// FIXME: not need vrect next field here?

	vec3_t vieworg;
} refdef_t;

// TODO: Implement

extern	refdef_t	r_refdef;



//
// refresh
//

extern vec3_t	r_origin, vpn, vright, vup;

extern	struct texture_s* r_notexture_mip;



void R_Init( void );
void R_InitTextures( void );


void R_SetStackBase( void );

// TODO: Implement