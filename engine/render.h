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

typedef struct
{
	byte					mouthopen;		// 0 = mouth closed, 255 = mouth agape
	byte					sndcount;		// counter for running average
	int						sndavg;			// running average
} mouth_t;


typedef struct cl_entity_s
{
	int				index;      // Index into cl_entities

	//FF: player_info_t* scoreboard;

	// TODO: Implement

	qboolean		resetlatched; //FF: maybe "nointerp"?

	// TODO: Implement

	entity_state_t	baseline;		// to fill in defaults in updates

	// Actual render position and angles
	vec3_t			origin;
	vec3_t			angles;

	int				rendermode;
	int				renderamt;
	color24			rendercolor;
	int				renderfx;

	struct model_s* model;			// cl.model_precache[ baseline.modelindex ];  all visible entities have a model
	struct efrag_s* efrags;			// linked list of efrags

	float			frame;

	float			syncbase;

	byte*			colormap;
	int				effects;
	int				skin;
	int				visframe;

	// TODO: Implement

	int				movetype;
	float			animtime;
	float			framerate;
	int				body;
	int				sequence;

	byte			controller[4];
	byte			blending[2];

	float			scale;
	float			lastmove;
	float			prevanimtime;
	vec3_t			prevorigin;
	vec3_t			prevangles;
	float			prevframe;
	float			sequencetime;
	int				prevsequence;
	byte			prevseqblending[2];
	byte			prevcontroller[4];
	byte			prevblending[2];

	// TODO: Implement

	colorVec		cvFloorColor;

	mouth_t			mouth;			// For synchronizing mouth movements.

	// TODO: Implement

} cl_entity_t;

typedef struct tempent_s
{
	int			flags;
	float		die;
	float		frameMax;
	float		x;
	float		y;
	float		z;
	float		fadeSpeed;
	int			hitSound;
	struct tempent_s* next;
	//FF: there must be another field (4 bytes)
	// TODO: Implement
	cl_entity_t	entity;
} TEMPENTITY;

typedef enum {
	pt_static,
	pt_grav,
	pt_slowgrav,
	pt_fire,
	pt_explode,
	pt_explode2,
	pt_blob,
	pt_blob2,
	pt_vox_slowgrav,
	pt_vox_grav
} ptype_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct particle_s
{
// driver-usable fields
	vec3_t		org;
	short		color;
	short		packedColor;
// drivers never touch the following fields
	struct particle_s	*next;
	vec3_t		vel;
	float		ramp;
	float		die;
	ptype_t		type;
} particle_t;

//====================================================

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

// R_PART.C

extern cvar_t tracerSpeed;
extern cvar_t tracerOffset;
extern cvar_t tracerLength;
extern cvar_t tracerRed;
extern cvar_t tracerGreen;
extern cvar_t tracerBlue;
extern cvar_t tracerAlpha;

extern cvar_t egon_amplitude;

void R_InitParticles( void );
void R_ClearParticles( void );
void R_DrawParticles( void );

particle_t* R_TracerParticles( vec_t* org, vec_t* vel, float life );

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