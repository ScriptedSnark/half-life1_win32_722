// refresh.h -- public interface to refresh functions



// TODO: Implement


//=============================================================================

typedef struct efrag_s
{
	struct mleaf_s* leaf;
	struct efrag_s* leafnext;
	struct cl_entity_s* entity;
	struct efrag_s* entnext;
} efrag_t;


typedef struct cl_entity_s
{


	entity_state_t			baseline;		// to fill in defaults in updates



	struct model_s* model;


	colorVec				cvFloorColor;
} cl_entity_t;

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



void R_NewMap( void );


void R_ParseParticleEffect( void );



void R_SetStackBase( void );

// TODO: Implement