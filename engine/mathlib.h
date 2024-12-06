// mathlib.h

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

struct mplane_s;

extern vec3_t vec3_origin;
extern	int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

// Use this definition globally
#define	EQUAL_EPSILON	0.001

#define clamp(val, min, max) ( ((val) > (max)) ? (max) : ( ((val) < (min)) ? (min) : (val) ) )

#define DotProduct(x, y) (x[0] * y[0] + x[1] * y[1] + x[2] * y[2])
#define VectorSubtract(a, b, c) {c[0] = a[0] - b[0]; c[1] = a[1] - b[1]; c[2] = a[2] - b[2];}
#define VectorAdd(a, b, c) {c[0] = a[0] + b[0]; c[1] = a[1] + b[1]; c[2] = a[2] + b[2];}
#define VectorCopy(a, b) {b[0] = a[0]; b[1] = a[1]; b[2] = a[2];}

// TODO: Implement

void AngleVectors(vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);

float	anglemod(float a);