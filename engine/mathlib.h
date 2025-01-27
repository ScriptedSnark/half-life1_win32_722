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

// TODO: Implement

int VectorCompare(const vec3_t v1, const vec3_t v2);

// TODO: Implement

void R_ConcatRotations( float in1[3][3], float in2[3][3], float out[3][3] );

// TODO: Implement

float Length( const vec_t* v );
void CrossProduct( const vec_t* v1, const vec_t* v2, vec_t* cross );
float VectorNormalize( vec_t* v );		// returns vector length

// TODO: Implement

void AngleVectors( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up );

void BOPS_Error( void );
int BoxOnPlaneSide( vec_t * emins, vec_t * emaxs, struct mplane_s* p );

float	anglemod(float a);