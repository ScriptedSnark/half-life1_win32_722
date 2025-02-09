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

#define ENTITY_NORMAL		0
#define ENTITY_BEAM			1

typedef struct cl_entity_s
{
	int				index;      // Index into cl_entities

	struct player_info_s* scoreboard;

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

	struct mnode_s* topnode;		// for bmodels, first world node that splits bmodel, or NULL if not split

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

	colorVec		cvFloorColor;

	// Attachment points
	vec3_t			attachment[4];

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

#if defined ( GLQUAKE )
extern float r_blend; // Global blending factor for the current entity
#else
extern int r_blend;
#endif



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
void R_RunParticleEffect( vec_t* org, vec_t* dir, int color, int count );
void R_RocketTrail( vec_t* start, vec_t* end, int type );
void R_EntityParticles( cl_entity_t* ent );
void R_ParticleExplosion( vec_t* org );
void R_ParticleExplosion2( vec_t* org, int colorStart, int colorLength );
void R_BlobExplosion(vec_t* org);

void R_PushDlights( void );

// R_PART.C

extern struct model_s* cl_sprite_dot;
extern struct model_s* cl_sprite_lightning;
extern struct model_s* cl_sprite_white;
extern struct model_s* cl_sprite_glow;
extern struct model_s* cl_sprite_ricochet;
extern struct model_s* cl_sprite_shell;

extern struct model_s* cl_sprite_muzzleflash[3];

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

void R_FlickerParticles( vec_t* org );
particle_t* R_TracerParticles( vec_t* org, vec_t* vel, float life );
void R_SparkStreaks( vec_t* pos, int count, int velocityMin, int velocityMax );
void R_StreakSplash( vec_t* pos, vec_t* dir, int color, int count, float speed, int velocityMin, int velocityMax );
void R_LavaSplash( vec_t* org );
void R_LargeFunnel( vec_t* org, int reverse );
void R_TeleportSplash( vec_t* org );
void R_ShowLine( vec_t* start, vec_t* end );
void R_BloodStream( vec_t* org, vec_t* dir, int pcolor, int speed );
void R_Blood( vec_t* org, vec_t* dir, int pcolor, int speed );

// Beams
struct beam_s* R_BeamAlloc( void );
void R_BeamSetup( struct beam_s* pbeam, vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed );
struct beam_s* R_BeamLightning( vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed );

int ScreenTransform( vec_t* point, vec_t* screen );

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