// mathlib.c -- math primitives

#include <math.h>
#include "quakedef.h"

void Sys_Error( char* error, ... );

/*-----------------------------------------------------------------*/

vec3_t vec3_origin = { 0, 0, 0 };
int nanmask = 255 << 23;


float anglemod( float a )
{
#if 0
	if (a >= 0)
		a -= 360 * (int)(a / 360);
	else
		a += 360 * (1 + (int)(-a / 360));
#endif
	a = (360.0 / 65536) * ((int)(a * (65536 / 360.0)) & 65535);
	return a;
}

/*
==================
BOPS_Error

Split out like this for ASM to call.
==================
*/
void BOPS_Error( void )
{
	Sys_Error("BoxOnPlaneSide:  Bad signbits");
}

// TODO: Implement

void AngleVectors( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * (M_PI * 2 / 360.0);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI * 2 / 360.0);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI * 2 / 360.0);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}
	if (up)
	{
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}

void AngleVectorsTranspose( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * (M_PI * 2 / 360.0);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI * 2 / 360.0);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI * 2 / 360.0);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = (sr * sp * cy + cr * -sy);
		forward[2] = (cr * sp * cy + -sr * -sy);
	}
	if (right)
	{
		right[0] = cp * sy;
		right[1] = (sr * sp * sy + cr * cy);
		right[2] = (cr * sp * sy + -sr * cy);
	}
	if (up)
	{
		up[0] = -sp;
		up[1] = sr * cp;
		up[2] = cr * cp;
	}
}

void AngleMatrix( const vec_t* angles, float(*matrix)[4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles[ROLL] * (M_PI * 2 / 360.0);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[YAW] * (M_PI * 2 / 360.0);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[PITCH] * (M_PI * 2 / 360.0);
	sr = sin(angle);
	cr = cos(angle);

	matrix[0][0] = cr * cp;
	matrix[0][1] = (sy * sr) * cp - cy * sp;
	matrix[0][2] = (cy * sr) * cp + sy * sp;
	matrix[0][3] = 0.0f;

	matrix[1][0] = cr * sp;
	matrix[1][1] = (sy * sr) * sp + cy * cp;
	matrix[1][2] = (cy * sr) * sp - sy * cp;
	matrix[1][3] = 0.0f;

	matrix[2][0] = -sr;
	matrix[2][1] = sy * cr;
	matrix[2][2] = cy * cr;
	matrix[2][3] = 0.0f;
}

void VectorTransform( const vec_t* in1, float(*in2)[4], vec_t* out )
{
	out[0] = in1[0] * (*in2)[0] + in1[1] * (*in2)[1] + in1[2] * (*in2)[2] + (*in2)[3];
	out[1] = in1[0] * (*in2)[4] + in1[1] * (*in2)[5] + in1[2] * (*in2)[6] + (*in2)[7];
	out[2] = in1[0] * (*in2)[8] + in1[1] * (*in2)[9] + in1[2] * (*in2)[10] + (*in2)[11];
}


int VectorCompare( const vec_t* v1, const vec_t* v2 )
{
	int i;

	for (i = 0; i < 3; i++)
		if (v1[i] != v2[i])
			return 0;

	return 1;
}

void VectorMA( const vec_t* veca, float scale, const vec_t* vecb, vec_t* vecc )
{
	vecc[0] = veca[0] + scale * vecb[0];
	vecc[1] = veca[1] + scale * vecb[1];
	vecc[2] = veca[2] + scale * vecb[2];
}

// TODO: Implement

void CrossProduct( const vec_t* v1, const vec_t* v2, vec_t* cross )
{
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

double sqrt(double x);

float Length( const vec_t* v )
{
	int		i;
	float	length;

	length = 0;
	for (i = 0; i < 3; i++)
		length += v[i] * v[i];
	length = sqrt(length);		// FIXME

	return length;
}

float VectorNormalize( vec_t* v )
{
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);		// FIXME

	if (length)
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;

}


// TODO: Implement

void VectorScale( const vec_t* in, vec_t scale, vec_t* out )
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

// TODO: Implement

void VectorMatrix( vec_t* forward, vec_t* right, vec_t* up )
{
	vec3_t tmp;

	if (forward[0] == 0 && forward[1] == 0)
	{
		right[0] = 1;
		right[1] = 0;
		right[2] = 0;
		up[0] = -forward[2];
		up[1] = 0;
		up[2] = 0;
		return;
	}

	tmp[0] = 0; tmp[1] = 0; tmp[2] = 1;
	CrossProduct(forward, tmp, right);
	VectorNormalize(right);
	CrossProduct(right, forward, up);
	VectorNormalize(up);
}

// TODO: Implement


/*
================
R_ConcatRotations
================
*/
void R_ConcatRotations( float in1[3][3], float in2[3][3], float out[3][3] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
		in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
		in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
		in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
		in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
		in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
		in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
		in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
		in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
		in1[2][2] * in2[2][2];
}


/*
================
R_ConcatTransforms
================
*/
void R_ConcatTransforms( float in1[3][4], float in2[3][4], float out[3][4] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
		in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
		in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
		in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
		in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
		in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
		in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
		in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
		in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
		in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
		in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
		in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
		in1[2][2] * in2[2][3] + in1[2][3];
}

// TODO: Implement