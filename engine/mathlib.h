// mathlib.h

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];	// x,y,z,w
typedef vec_t vec5_t[5];

typedef short vec_s_t;
typedef vec_s_t vec3s_t[3];
typedef vec_s_t vec4s_t[4];	// x,y,z,w
typedef vec_s_t vec5s_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define ROLL	2
#define PITCH	0
#define YAW		1

extern vec3_t vec3_origin;
extern	int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

#define DotProduct(x, y) (x[0] * y[0] + x[1] * y[1] + x[2] * y[2])

// Use this definition globally
#define	EQUAL_EPSILON	0.001

#define clamp(val, min, max) ( ((val) > (max)) ? (max) : ( ((val) < (min)) ? (min) : (val) ) )

#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorClear(a) {(a)[0]=0.0;(a)[1]=0.0;(a)[2]=0.0;}

void VectorMA( const vec_t* veca, float scale, const vec_t* vecb, vec_t* vecc );

// TODO: Implement

int VectorCompare( const vec_t* v1, const vec_t* v2 );

// TODO: Implement

void R_ConcatRotations( float in1[3][3], float in2[3][3], float out[3][3] );
void R_ConcatTransforms( float in1[3][4], float in2[3][4], float out[3][4] );

// Here are some "manual" INLINE routines for doing floating point to integer conversions
extern short new_cw, old_cw;

typedef union DLONG {
	int		i[2];
	double	d;
	float	f;
	} DLONG;

extern DLONG	dlong;

#ifdef _WIN32
void __inline set_fpu_cw( void )
{
	_asm	
	{		wait
			fnstcw	old_cw
			wait
			mov		ax, word ptr old_cw
			or		ah, 0xc
			mov		word ptr new_cw,ax
			fldcw	new_cw
	}
}

int __inline quick_ftol( float f )
{
	_asm {
		// Assumes that we are already in chop mode, and only need a 32-bit int
		fld		DWORD PTR f
		fistp	DWORD PTR dlong
	}
	return dlong.i[0];
}

void __inline restore_fpu_cw( void )
{
	_asm	fldcw	old_cw
}
#else
#define set_fpu_cw() /* */
#define quick_ftol(f) ftol(f)
#define restore_fpu_cw() /* */
#endif

// TODO: Implement

float Length( const vec_t* v );
void CrossProduct( const vec_t* v1, const vec_t* v2, vec_t* cross );
float VectorNormalize( vec_t* v );		// returns vector length

void VectorScale( const vec_t* in, vec_t scale, vec_t* out );

// TODO: Implement

void AngleVectors( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up );
void AngleVectorsTranspose( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up );

void BOPS_Error( void );
int BoxOnPlaneSide( vec_t * emins, vec_t * emaxs, struct mplane_s* p );


void AngleMatrix( const vec_t* angles, float(*matrix)[4] );
void VectorTransform( const vec_t* in1, float(*in2)[4], vec_t* out );

void VectorMatrix( vec_t* forward, vec_t* right, vec_t* up );

float	anglemod( float a );