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
	int				index;      // Index into cl_entities

	// TODO: Implement

	entity_state_t	baseline;		// to fill in defaults in updates

	// Actual render position and angles
	vec3_t			origin;
	vec3_t			angles;

	int				rendermode;
	int				renderamt;
	color24			rendercolor;
	int				renderfx;

	struct model_s* model;			// cl.model_precache[ curstate.modelindes ];  all visible entities have a model
	struct efrag_s* efrags;			// linked list of efrags

	float			frame;

	// TODO: Implement

	byte* colormap;
	int				effects;
	int				skin;

	// TODO: Implement

	int				movetype;
	float			animtime;
	float			framerate;
	int				body;

	// TODO: Implement

	byte			controller[4];
	byte			blending[4];

	float			scale;

	// TODO: Implement

	colorVec		cvFloorColor;
} cl_entity_t;

// TODO: Implement


typedef struct
{
	vrect_t		vrect;								// subwindow in video for refresh
													// FIXME: not need vrect next field here?

	// TODO: Implement

	vec3_t		vieworg;
	vec3_t		viewangles;

	color24		ambientlight;
} refdef_t;

// TODO: Implement

extern	refdef_t	r_refdef;



//
// refresh
//

extern vec3_t	r_origin, vpn, vright, vup;

extern struct texture_s* r_notexture_mip;

extern cl_entity_t r_worldentity;


void R_Init( void );
void R_InitTextures( void );
void R_RenderView( void );		// must set r_refdef first
void R_ViewChanged( vrect_t* pvrect, int lineadj, float aspect );

void R_InitSky( void );



void R_NewMap( void );


void R_ParseParticleEffect( void );



void R_PushDlights( void );
void R_InitParticles( void );
void R_ClearParticles( void );
void R_DrawParticles( void );

extern qboolean r_intentities;

//
// Skybox
//

void R_LoadSkys( void );
void R_ClearSkyBox( void );



//
// surface cache related
//

extern qboolean	r_cache_thrash;	// set if thrashing the surface cache

void D_FlushCaches( void );

void R_SetStackBase( void );

// TODO: Implement