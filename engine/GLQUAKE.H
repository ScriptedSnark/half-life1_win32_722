// disable data conversion warnings

#ifndef GLQUAKE_H
#define GLQUAKE_H
#pragma once

#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA
#pragma warning(disable : 4305)     // truncation from 'const double ' to 'float '

typedef struct auxvert_s
{
	float fv[3];		// viewspace x, y
} auxvert_t;

typedef struct alight_s
{
	int			ambientlight;	// clip at 128
	int			shadelight;		// clip at 192 - ambientlight
	vec3_t		color;
	float*		plightvec;
} alight_t;

void GL_BeginRendering( int* x, int* y, int* width, int* height );
void GL_EndRendering( void );

extern	int texture_extension_number;
extern	int	texture_mode;

extern	float	gldepthmin, gldepthmax;

typedef enum
{
	GLT_SYSTEM,
	GLT_DECAL,
	GLT_HUDSPRITE,
	GLT_STUDIO,
	GLT_WORLD,
	GLT_SPRITE
} GL_TEXTURETYPE;

void GL_Upload32( unsigned int* data, int width, int height, qboolean mipmap, int iType );
void GL_Upload16( unsigned char* data, int width, int height, qboolean mipmap, int iType, unsigned char* pPal );
int GL_LoadTexture( char* identifier, GL_TEXTURETYPE textureType, int width, int height, unsigned char* data, int mipmap, int iType, unsigned char* pPal );
int GL_LoadPicTexture( qpic_t* pic, char* pszName );

void GL_UnloadTextures( void );


void GL_SelectTexture( GLenum target );



void GLBeginHud( void );
void GLFinishHud( void );
void DrawWedge( float centerx, float centery, float angle1, float angle2, float radius );



extern	int glx, gly, glwidth, glheight;

#define BACKFACE_EPSILON	0.01

#define TEX_TYPE_NONE				0
#define TEX_TYPE_ALPHA				1
#define TEX_TYPE_LUM				2
#define TEX_TYPE_ALPHA_GRADIENT		3
#define TEX_TYPE_RGBA				4

#define TEX_IS_ALPHA(type)		((type) == TEX_TYPE_ALPHA || (type) == TEX_TYPE_ALPHA_GRADIENT || (type) == TEX_TYPE_RGBA)



//
// screen size info
//
extern refdef_t	r_refdef;
extern mleaf_t* r_viewleaf, * r_oldviewleaf;
extern texture_t* r_notexture_mip;
extern	int		d_lightstylevalue[256];	// 8.8 fraction of base light value


extern	int	currenttexture;
extern	int	cnttextures[2];

extern	int	skytexturenum;		// index in cl.loadmodel, not gl texture object

extern	cvar_t	r_norefresh;
extern	cvar_t	r_speeds;


extern	cvar_t	r_wadtextures;


extern	cvar_t	gl_wateramp;
extern	cvar_t	gl_dither;
extern	cvar_t	gl_spriteblend;


extern	int		mirrortexturenum;	// quake texturenum, not gltexturenum
extern	qboolean mirror;
extern	mplane_t* mirror_plane;

extern	float	r_world_matrix[16];
extern	float	r_base_world_matrix[16];
extern	float	gProjectionMatrix[16];
extern	float	gWorldToScreen[16];
extern	float	gScreenToWorld[16];

extern	const char* gl_vendor;
extern	const char* gl_renderer;
extern	const char* gl_version;
extern	const char* gl_extensions;



void GL_Bind( int texnum );

// Multitexture
#define    TEXTURE0_SGIS				0x835E
#define    TEXTURE1_SGIS				0x835F


extern qboolean gl_mtexable;



void GL_DisableMultitexture( void );
void GL_EnableMultitexture( void );


//
// gl_rsurf.c
//
void R_DrawBrushModel( cl_entity_t* e );
void R_DrawWorld( void );
void GL_BuildLightmaps( void );
void R_DecalInit( void );

#endif // GLQUAKE_H