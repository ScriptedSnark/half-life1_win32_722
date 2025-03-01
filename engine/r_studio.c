// r_studio.c: routines for setting up to draw 3DStudio models 

#include "quakedef.h"
#include "pr_cmds.h"
#include "view.h"
#include "CL_TENT.H"
#include "customentity.h"
#include "r_local.h"
#include "r_triangle.h"
#include "r_studio.h"

// Hulls & planes
#define STUDIO_NUM_HULLS	128

// Pointer to header block for studio model data
studiohdr_t* pstudiohdr;

// TODO: Implement

vec3_t			r_colormix;
colorVec		r_icolormix;
vec3_t			r_blightvec[MAXSTUDIOBONES];	// light vectors in bone reference frames

// Model to world transformation
float			rotationmatrix[3][4];

// Concatenated bone and light transforms
float			bonetransform[MAXSTUDIOBONES][3][4];
float			lighttransform[MAXSTUDIOBONES][3][4];

// TODO: Implement

// Vert data, position and lighting
auxvert_t		auxverts[MAXSTUDIOVERTS];
vec3_t			lightvalues[MAXSTUDIOVERTS];

// TODO: Implement

// Do interpolation?
int r_dointerp = 1;

//
// Global studio hull/clipnode/plane data to
// copy the cached ones
int				studio_hull_hitgroup[STUDIO_NUM_HULLS];

// TODO: Implement

extern	vec3_t	shadevector;

// TODO: Implement







// TODO: Implement

// Pointers to current body part and submodel
mstudiobodyparts_t* pbodypart;
mstudiomodel_t* psubmodel;
mstudiomesh_t* pmesh;

// TODO: Implement

// Chrome and light data

int				chrome[MAXSTUDIOVERTS][2];		// texture coords for surface normals
int				g_NormalIndex[MAXSTUDIOVERTS];
int				chromeage[MAXSTUDIOBONES];		// last time chrome vectors were updated
vec3_t			r_chromeup[MAXSTUDIOBONES];		// chrome vector "up" in bone reference frames
vec3_t			r_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames
#define MAXLOCALLIGHTS 3
int				numlights;
dlight_t*		locallight[MAXLOCALLIGHTS];
int				locallinearlight[MAXLOCALLIGHTS][3];
float			locallightR2[MAXLOCALLIGHTS];
float			lightpos[MAXSTUDIOVERTS][3][4];
vec_t			lightbonepos[MAXSTUDIOBONES][3][3];
int				lightage[MAXSTUDIOBONES];					// last time lights were updated

#if defined( GLQUAKE )
auxvert_t* pauxverts;
#endif

// Software's drawstyle for debugging
// the studio model
int				drawstyle;

#if defined( GLQUAKE )
int				r_ambientlight;					// ambient world light
float			r_shadelight;					// direct world light

int				r_amodels_drawn;
int				r_smodels_total;				// cookie
#endif

#if !defined( GLQUAKE )
int				r_smodels_total;				// cookie

// TODO: Implement
#endif

void R_StudioTransformVector( vec_t* in, vec_t* out );
int SignbitsForPlane( mplane_t* out );

#if defined( GLQUAKE )
/*
===========
R_StudioCheckBBox

Checks if entity's bbox is in the view frustum
===========
*/
qboolean R_StudioCheckBBox( void )
{
	mplane_t	plane;
	vec3_t		mins, maxs;
	int			i;
	mstudioseqdesc_t* pseqdesc;

	// Fake bboxes for models
	static vec3_t gFakeHullMins = { -16, -16, -16 };
	static vec3_t gFakeHullMaxs = { 16, 16, 16 };

	// check if we have valid mins\maxs
	if (!VectorCompare(vec3_origin, pstudiohdr->bbmin))
	{
		// clipping bounding box
		VectorAdd(currententity->origin, pstudiohdr->bbmin, mins);
		VectorAdd(currententity->origin, pstudiohdr->bbmax, maxs);
	}
	else if (!VectorCompare(vec3_origin, pstudiohdr->min))
	{
		// movement bounding box
		VectorAdd(currententity->origin, pstudiohdr->min, mins);
		VectorAdd(currententity->origin, pstudiohdr->max, maxs);
	}
	else
	{
		// fake bounding box
		VectorAdd(currententity->origin, gFakeHullMins, mins);
		VectorAdd(currententity->origin, gFakeHullMaxs, maxs);
	}

	// check sequence range
	if (currententity->sequence >= pstudiohdr->numseq)
		currententity->sequence = 0;

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

// compute a full bounding box
	for (i = 0; i < 8; i++)
	{
		vec3_t p1, p2;
		p1[0] = (i & 1) ? pseqdesc->bbmin[0] : pseqdesc->bbmax[0];
		p1[1] = (i & 2) ? pseqdesc->bbmin[1] : pseqdesc->bbmax[1];
		p1[2] = (i & 4) ? pseqdesc->bbmin[2] : pseqdesc->bbmax[2];

		R_StudioTransformVector(p1, p2);

		if (mins[0] > p2[0]) mins[0] = p2[0];
		if (maxs[0] < p2[0]) maxs[0] = p2[0];
		if (mins[1] > p2[1]) mins[1] = p2[1];
		if (maxs[1] < p2[1]) maxs[1] = p2[1];
		if (mins[2] > p2[2]) mins[2] = p2[2];
		if (maxs[2] < p2[2]) maxs[2] = p2[2];
	}

	VectorCopy(vpn, plane.normal);
	plane.dist = DotProduct(plane.normal, r_origin);
	plane.type = PLANE_ANYZ;
	plane.signbits = SignbitsForPlane(&plane);

	if (BoxOnPlaneSide(mins, maxs, &plane) == 2)
		return FALSE;

	return TRUE;
}
#else
/*
===========
R_StudioCheckBBox

Checks if entity's bbox is in the view frustum
===========
*/
qboolean R_StudioCheckBBox( void )
{
	// TODO: Implement
	return TRUE;
}
#endif

// Get number of body variations
int R_StudioBodyVariations( model_t* model )
{
	studiohdr_t* pstudiohdr;
	mstudiobodyparts_t* pbodypart;
	int		i, count;

	if (model->type != mod_studio)
		return 0;

	pstudiohdr = (studiohdr_t*)Mod_Extradata(model);
	if (!pstudiohdr)
		return 0;

	count = 1;

	pbodypart = (mstudiobodyparts_t*)((byte*)pstudiohdr + pstudiohdr->bodypartindex);
	for (i = 0; i < pstudiohdr->numbodyparts; i++, pbodypart++)
	{
		count *= pbodypart->nummodels;
	}

	return count;
}

// TODO: Implement



// TODO: Implement

/*
================
R_StudioTransformVector
================
*/
#if !defined( GLQUAKE )
void R_StudioTransformVector( vec_t* in, vec_t* out )
{
	out[0] = DotProduct(in, aliastransform[0]) + aliastransform[0][3];
	out[1] = DotProduct(in, aliastransform[1]) + aliastransform[1][3];
	out[2] = DotProduct(in, aliastransform[2]) + aliastransform[2][3];
}
#else
void R_StudioTransformVector( vec_t* in, vec_t* out )
{
	out[0] = DotProduct(in, rotationmatrix[0]) + rotationmatrix[0][3];
	out[1] = DotProduct(in, rotationmatrix[1]) + rotationmatrix[1][3];
	out[2] = DotProduct(in, rotationmatrix[2]) + rotationmatrix[2][3];
}
#endif

// TODO: Implement

float h_scale = 1.5;

/*
================
R_StudioSetUpTransform
================
*/
void R_StudioSetUpTransform( int trivial_accept )
{
	int				i;
	vec3_t			angles;
	vec3_t			modelpos;

	// tweek model origin	
		//for (i = 0; i < 3; i++)
		//	modelpos[i] = currententity->origin[i];

	VectorCopy(currententity->origin, modelpos);

	// TODO: should really be stored with the entity instead of being reconstructed
	// TODO: should use a look-up table
	// TODO: could cache lazily, stored in the entity
	angles[ROLL] = currententity->angles[ROLL];
	angles[PITCH] = currententity->angles[PITCH];
	angles[YAW] = currententity->angles[YAW];

	//Con_DPrintf("Angles %4.2f prev %4.2f for %i\n", angles[PITCH], currententity->index);
	//Con_DPrintf("movetype %d %d\n", currententity->movetype, currententity->aiment);
	if (currententity->movetype != MOVETYPE_NONE)
	{
		float			f = 0;
		float			d;

		// don't do it if the goalstarttime hasn't updated in a while.

		// NOTE:  Because we need to interpolate multiplayer characters, the interpolation time limit
		//  was increased to 1.0 s., which is 2x the max lag we are accounting for.

		if (cl.time < (currententity->animtime + 1.0) &&
			(currententity->animtime != currententity->prevanimtime))
		{
			f = (cl.time - currententity->animtime) / (currententity->animtime - currententity->prevanimtime);
			//Con_DPrintf("%4.2f %.2f %.2f\n", f, currententity->animtime, cl.time);
		}

		if (r_dointerp)
		{
			// ugly hack to interpolate angle, position. current is reached 0.1 seconds after being set
			f = f - 1.0;
		}
		else
		{
			f = 0;
		}

		for (i = 0; i < 3; i++)
		{
			modelpos[i] += (currententity->origin[i] - currententity->prevorigin[i]) * f;
		}

		// NOTE:  Because multiplayer lag can be relatively large, we don't want to cap
		//  f at 1.5 anymore.
		//if (f > -1.0 && f < 1.5) {}

//			Con_DPrintf("%.0f %.0f\n", currententity->msg_angles[0][YAW], currententity->msg_angles[1][YAW]);
		for (i = 0; i < 3; i++)
		{
			float ang1, ang2;

			ang1 = currententity->angles[i];
			ang2 = currententity->prevangles[i];

			d = ang1 - ang2;
			if (d > 180)
			{
				d -= 360;
			}
			else if (d < -180)
			{
				d += 360;
			}

			angles[i] += d * f;
		}
		//Con_DPrintf("%.3f \n", f);
	}

	//Con_DPrintf("%.0f %0.f %0.f\n", modelpos[0], modelpos[1], modelpos[2]);
	//Con_DPrintf("%.0f %0.f %0.f\n", angles[0], angles[1], angles[2]);

	angles[PITCH] = -angles[PITCH];
	AngleMatrix(angles, rotationmatrix);

#if !defined( GLQUAKE )
	// TODO: Implement
#endif

	rotationmatrix[0][3] = modelpos[0];
	rotationmatrix[1][3] = modelpos[1];
	rotationmatrix[2][3] = modelpos[2];
}

// rotate by the inverse of the matrix
void VectorIRotate( vec_t* in1, float(*in2)[4], vec_t* out )
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

void AngleQuaternion( vec_t* angles, vec_t* quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy; // X
	quaternion[1] = cr * sp * cy + sr * cp * sy; // Y
	quaternion[2] = cr * cp * sy - sr * sp * cy; // Z
	quaternion[3] = cr * cp * cy + sr * sp * sy; // W
}

void QuaternionMatrix( vec_t* quaternion, float(*matrix)[4] )
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

float	omega, cosom, sinom, sclp, sclq;
void QuaternionSlerp( vec_t* p, vec_t* q, float t, vec_t* qt )
{
	int i;
	
	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i] - q[i]) * (p[i] - q[i]);
		b += (p[i] + q[i]) * (p[i] + q[i]);
	}
	if (a > b)
	{
		for (i = 0; i < 4; i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ((1.0 + cosom) > 0.000001)
	{
		if ((1.0 - cosom) > 0.000001)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0 - t) * omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin((1.0 - t) * (0.5 * M_PI));
		sclq = sin(t * (0.5 * M_PI));
		for (i = 0; i < 3; i++)
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

/*
====================
R_StudioCalcBoneAdj

Compute bone adjustments ( bone controllers )
====================
*/
void R_StudioCalcBoneAdj( float dadt, float* adj, const unsigned char* pcontroller1, const unsigned char* pcontroller2, unsigned char mouthopen )
{
	int					i, j;
	float				value;
	mstudiobonecontroller_t* pbonecontroller;

	pbonecontroller = (mstudiobonecontroller_t*)((byte*)pstudiohdr + pstudiohdr->bonecontrollerindex);

	for (j = 0; j < pstudiohdr->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			// check for 360% wrapping
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				if (abs(pcontroller1[i] - pcontroller2[i]) > 128)
				{
					int a, b;
					a = (pcontroller1[j] + 128) % 256;
					b = (pcontroller2[j] + 128) % 256;
					value = ((a * dadt) + (b * (1 - dadt)) - 128) * (360.0 / 256.0) + pbonecontroller[j].start;
				}
				else
				{
					value = (pcontroller1[i] * dadt + (pcontroller2[i]) * (1.0 - dadt)) * (360.0 / 256.0) + pbonecontroller[j].start;
				}
			}
			else
			{
				value = (pcontroller1[i] * dadt + pcontroller2[i] * (1.0 - dadt)) / 255.0;
				if (value < 0) value = 0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
			//Con_DPrintf("%d %d %f : %f\n", currententity->controller[j], currententity->prevcontroller[j], value, dadt);
		}
		else
		{
			// mouth hardcoded at controller 4
			value = mouthopen / 64.0;
			if (value > 1.0) value = 1.0;
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			//Con_DPrintf("%d %f\n", mouthopen, value);
		}
		switch (pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			adj[j] = value * (M_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			adj[j] = value;
			break;
		}
	}
}

void R_StudioCalcBoneQuaterion( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t* panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j + 3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j + 3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j + 3]);
			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k + 1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k + 2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
			angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
		}

		if (pbone->bonecontroller[j + 3] != -1)
		{
			angle1[j] += adj[pbone->bonecontroller[j + 3]];
			angle2[j] += adj[pbone->bonecontroller[j + 3]];
		}
	}

	if (!VectorCompare(angle1, angle2))
	{
		AngleQuaternion(angle1, q1);
		AngleQuaternion(angle2, q2);
		QuaternionSlerp(q1, q2, s, q);
	}
	else
	{
		AngleQuaternion(angle1, q);
	}
}

void R_StudioCalcBonePosition( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos )
{
	int					j, k;
	mstudioanimvalue_t* panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j]);
			/*
			if (i == 0 && j == 0)
				Con_DPrintf("%d  %d:%d  %f\n", frame, panimvalue->num.valid, panimvalue->num.total, s);
			*/

			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k + 1].value * (1.0 - s) + s * panimvalue[k + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k + 1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if (pbone->bonecontroller[j] != -1)
		{
			pos[j] += adj[pbone->bonecontroller[j]];
		}
	}
}

float CL_StudioEstimateInterpolant( void )
{
	float dadt = 1.0;

	if (r_dointerp && (currententity->animtime >= currententity->prevanimtime + 0.01))
	{
		dadt = (cl.time - currententity->animtime) / 0.1;
		if (dadt > 2.0)
		{
			dadt = 2.0;
		}
	}
	return dadt;
}

/*
====================
R_StudioCalcRotations

====================
*/
void R_StudioCalcRotations( vec3_t* pos, vec4_t* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f )
{
	int					i;
	int					frame;
	mstudiobone_t* pbone;

	float				s;
	float				adj[MAXSTUDIOCONTROLLERS];
	float				dadt;

	if (f > pseqdesc->numframes - 1)
	{
		f = 0;	// bah, fix this bug with changing sequences too fast
	}

	frame = (int)f;

	// Con_DPrintf("%d %.4f %.4f %.4f %.4f %d\n", currententity->sequence, cl.time, currententity->animtime, currententity->frame, f, frame);

	// Con_DPrintf("%f %f %f\n", currententity->angles[ROLL], currententity->angles[PITCH], currententity->angles[YAW]);

	// Con_DPrintf("frame %d %d\n", frame1, frame2);


	dadt = CL_StudioEstimateInterpolant();
	s = (f - frame);

	// add in programtic controllers
	pbone = (mstudiobone_t*)((byte*)pstudiohdr + pstudiohdr->boneindex);

	R_StudioCalcBoneAdj(dadt, adj, currententity->controller, currententity->prevcontroller, currententity->mouth.mouthopen);

	for (i = 0; i < pstudiohdr->numbones; i++, pbone++, panim++)
	{
		R_StudioCalcBoneQuaterion(frame, s, pbone, panim, adj, q[i]);

		R_StudioCalcBonePosition(frame, s, pbone, panim, adj, pos[i]);
		// if (0 && i == 0)
		//	Con_DPrintf("%d %d %d %d\n", currententity->sequence, frame, j, k);
	}

	if (pseqdesc->motiontype & STUDIO_X)
	{
		pos[pseqdesc->motionbone][0] = 0.0;
	}
	if (pseqdesc->motiontype & STUDIO_Y)
	{
		pos[pseqdesc->motionbone][1] = 0.0;
	}
	if (pseqdesc->motiontype & STUDIO_Z)
	{
		pos[pseqdesc->motionbone][2] = 0.0;
	}
}

/*
====================
R_GetAnim

====================
*/
mstudioanim_t* R_GetAnim( model_t* psubmodel, mstudioseqdesc_t* pseqdesc )
{
	mstudioseqgroup_t* pseqgroup;
	cache_user_t* paSequences;

	pseqgroup = (mstudioseqgroup_t*)((byte*)pstudiohdr + pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t*)((byte*)pstudiohdr + pseqdesc->animindex + pseqgroup->data);
	}

	paSequences = (cache_user_t*)psubmodel->submodels;

	if (paSequences == NULL)
	{
		paSequences = (cache_user_t*)calloc(sizeof(dmodel_t) * MAXSTUDIOGROUPS, MAXSTUDIOGROUPS); // UNDONE: leak!
		psubmodel->submodels = (dmodel_t*)paSequences;
	}

	if (!Cache_Check(&paSequences[pseqdesc->seqgroup]))
	{
		Con_DPrintf("loading %s\n", pseqgroup->name);
		COM_LoadCacheFile(pseqgroup->name, &paSequences[pseqdesc->seqgroup]);
	}
	return (mstudioanim_t*)((byte*)paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex);
}

/*
====================
R_StudioSlerpBones

====================
*/
void R_StudioSlerpBones( vec4_t* q1, vec3_t* pos1, vec4_t* q2, vec3_t* pos2, float s )
{
	int			i;
	vec4_t		q3;
	float		s1;

	if (s < 0) s = 0;
	else if (s > 1.0) s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < pstudiohdr->numbones; i++)
	{
		QuaternionSlerp(q1[i], q2[i], s, q3);
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

float CL_StudioEstimateFrame( mstudioseqdesc_t* pseqdesc )
{
	double				dfdt, f;

	if (r_dointerp)
	{
		dfdt = (cl.time - currententity->animtime) * currententity->framerate * pseqdesc->fps;
	}
	else
	{
		dfdt = 0;
	}

	if (pseqdesc->numframes <= 1)
	{
		f = 0;
	}
	else
	{
		f = (currententity->frame * (pseqdesc->numframes - 1)) / 256.0;
	}

	f += dfdt;

	if (pseqdesc->flags & STUDIO_LOOPING)
	{
		if (pseqdesc->numframes > 1)
		{
			f -= (int)(f / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
		}
		if (f < 0)
		{
			f += (pseqdesc->numframes - 1);
		}
	}
	else
	{
		if (f >= pseqdesc->numframes - 1.001)
		{
			f = pseqdesc->numframes - 1.001;
		}
		if (f < 0.0)
		{
			f = 0.0;
		}
	}
	return f;
}

void R_StudioSetupBones( void )
{
	int					i;
	double				f;

	mstudiobone_t* pbones;
	mstudioseqdesc_t* pseqdesc;
	mstudioanim_t* panim;

	static float		pos[MAXSTUDIOBONES][3];
	static vec4_t		q[MAXSTUDIOBONES];
	float				bonematrix[3][4];

	static float		pos2[MAXSTUDIOBONES][3];
	static vec4_t		q2[MAXSTUDIOBONES];

	if (currententity->sequence >= pstudiohdr->numseq)
	{
		currententity->sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

	f = CL_StudioEstimateFrame(pseqdesc);

	if (currententity->prevframe > f)
	{
		//Con_DPrintf("%f %f\n", currententity->prevframe, f);
	}

	panim = R_GetAnim(currententity->model, pseqdesc);
	R_StudioCalcRotations(pos, q, pseqdesc, panim, f);

	if (pseqdesc->numblends > 1)
	{
		float				s;
		float				dadt;

		pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;
		panim = R_GetAnim(currententity->model, pseqdesc) + pstudiohdr->numbones;
		R_StudioCalcRotations(pos2, q2, pseqdesc, panim, f);

		dadt = CL_StudioEstimateInterpolant();
		s = (currententity->blending[0] * dadt + currententity->prevblending[0] * (1.0 - dadt)) / 255.0;

		R_StudioSlerpBones(q, pos, q2, pos2, s);
	}

	if (r_dointerp &&
		currententity->sequencetime &&
		(currententity->sequencetime + 0.2 > cl.time) &&
		(currententity->prevsequence < pstudiohdr->numseq))
	{
		// blend from last sequence
		static float		pos1b[MAXSTUDIOBONES][3];
		static float		pos2b[MAXSTUDIOBONES][3];
		static vec4_t		q1b[MAXSTUDIOBONES];
		static vec4_t		q2b[MAXSTUDIOBONES];
		float				s;

		pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->prevsequence;
		panim = R_GetAnim(currententity->model, pseqdesc);
		// clip prevframe
		R_StudioCalcRotations(pos1b, q1b, pseqdesc, panim, currententity->prevframe);

		if (pseqdesc->numblends > 1)
		{
			pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->prevsequence;
			panim = R_GetAnim(currententity->model, pseqdesc) + pstudiohdr->numbones;
			R_StudioCalcRotations(pos2b, q2b, pseqdesc, panim, f);

			s = (currententity->prevseqblending[0]) / 255.0;
			R_StudioSlerpBones(q1b, pos1b, q2b, pos2b, s);
		}

		s = 1.0 - (cl.time - currententity->sequencetime) / 0.2;
		R_StudioSlerpBones(q, pos, q1b, pos1b, s);
	}
	else
	{
		//Con_DPrintf("prevframe = %4.2f\n", f);
		currententity->prevframe = f;
	}

	pbones = (mstudiobone_t*)((byte*)pstudiohdr + pstudiohdr->boneindex);

	for (i = 0; i < pstudiohdr->numbones; i++)
	{
		QuaternionMatrix(q[i], bonematrix);

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1)
		{
#if defined( GLQUAKE )
			R_ConcatTransforms(rotationmatrix, bonematrix, bonetransform[i]);
			R_ConcatTransforms(rotationmatrix, bonematrix, lighttransform[i]);
#else
			R_ConcatTransforms(aliastransform, bonematrix, bonetransform[i]);
			R_ConcatTransforms(rotationmatrix, bonematrix, lighttransform[i]);
#endif

			// Apply client-side effects to the transformation matrix
			CL_FxTransform(currententity, bonetransform[i][0]);
		}
		else
		{
			R_ConcatTransforms(bonetransform[pbones[i].parent], bonematrix, bonetransform[i]);
			R_ConcatTransforms(lighttransform[pbones[i].parent], bonematrix, lighttransform[i]);
		}
	}
}

void MatrixCopy( float(*in)[4], float(*out)[4] )
{
	memcpy(out, in, sizeof(float) * 3 * 4);
}

/*
====================
R_StudioDrawPlayer

Merge bones of a child model with current one
====================
*/
void R_StudioMergeBones( studiohdr_t* psubstudiohdr, model_t* psubmodel )
{
	int					i, j;
	double				f;

	mstudiobone_t* pbones, * psubbones;
	mstudioseqdesc_t* pseqdesc;
	mstudioanim_t* panim;

	static float		pos[MAXSTUDIOBONES][3];
	float				bonematrix[3][4];
	static vec4_t		q[MAXSTUDIOBONES];

	if (currententity->sequence >= pstudiohdr->numseq)
	{
		currententity->sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

	f = CL_StudioEstimateFrame(pseqdesc);

	if (currententity->prevframe > f)
	{
		//Con_DPrintf("%f %f\n", currententity->prevframe, f);
	}

	panim = R_GetAnim(psubmodel, pseqdesc);
	R_StudioCalcRotations(pos, q, pseqdesc, panim, f);

	pbones = (mstudiobone_t*)((byte*)pstudiohdr + pstudiohdr->boneindex);
	psubbones = (mstudiobone_t*)((byte*)psubstudiohdr + psubstudiohdr->boneindex);

	for (i = 0; i < pstudiohdr->numbones; i++)
	{
		for (j = 0; j < psubstudiohdr->numbones; j++)
		{
			if (_stricmp(pbones[i].name, psubbones[j].name) == 0)
			{
				MatrixCopy(bonetransform[j], bonetransform[i]);
				MatrixCopy(lighttransform[j], lighttransform[i]);
				break;
			}
		}
		if (j >= psubstudiohdr->numbones)
		{
			QuaternionMatrix(q[i], bonematrix);

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if (pbones[i].parent == -1)
			{
#if defined( GLQUAKE )
				R_ConcatTransforms(rotationmatrix, bonematrix, bonetransform[i]);
				R_ConcatTransforms(rotationmatrix, bonematrix, lighttransform[i]);
#else
				R_ConcatTransforms(aliastransform, bonematrix, bonetransform[i]);
				R_ConcatTransforms(rotationmatrix, bonematrix, lighttransform[i]);
#endif

				// Apply client-side effects to the transformation matrix
				CL_FxTransform(currententity, bonetransform[i][0]);
			}
			else
			{
				R_ConcatTransforms(bonetransform[pbones[i].parent], bonematrix, bonetransform[i]);
				R_ConcatTransforms(lighttransform[pbones[i].parent], bonematrix, lighttransform[i]);
			}
		}
	}
}

// TODO: Implement

void AnimationAutomove( const edict_t* pEdict, float flTime )
{
}

void GetBonePosition( const edict_t* pEdict, int iBone, float* rgflOrigin, float* rgflAngles )
{
	// TODO: Implement
}

// TODO: Implement

/*
====================
GetAttachment

Get the attachment origin and angles
====================
*/
void GetAttachment( const edict_t* pEdict, int iAttachment, float* rgflOrigin, float* rgflAngles )
{
	// TODO: Implement
}

// TODO: Implement

hull_t* R_StudioHull( model_t* pModel, float frame, int sequence, const vec_t* angles, const vec_t* origin, const vec_t* size, const byte* pcontroller, const byte* pblending, int* pNumHulls )
{
	// TODO: Implement
	return NULL;
}

int SV_HitgroupForStudioHull( int index )
{
	return studio_hull_hitgroup[index];
}

void R_StudioDrawHulls( void )
{
	// TODO: Implement
}

// TODO: Implement

void R_StudioDrawBones( void )
{
	// TODO: Implement
}


/*
====================
R_StudioTransformAuxVert

====================
*/
void R_StudioTransformAuxVert( auxvert_t* av, int bone, vec_t* vert )
{
	av->fv[0] = DotProduct(bonetransform[bone][0], vert)
		+ bonetransform[bone][0][3];
	av->fv[1] = DotProduct(bonetransform[bone][1], vert)
		+ bonetransform[bone][1][3];
	av->fv[2] = DotProduct(bonetransform[bone][2], vert)
		+ bonetransform[bone][2][3];
}

/*
====================
R_StudioLighting

====================
*/
void R_StudioLighting( float* lv, int bone, int flags, vec_t* normal )
{
	float illum;
	float lightcos;

	illum = r_ambientlight;

	if ((flags & STUDIO_NF_FLATSHADE) && drawstyle != 1)
	{
		illum += r_shadelight * 0.8;
	}
	else
	{
		float r;
		if (bone == -1)
			lightcos = DotProduct(normal, r_plightvec);
		else
			lightcos = DotProduct(normal, r_blightvec[bone]); // -1 colinear, 1 opposite

		if (lightcos > 1)
			lightcos = 1;

		r = v_lambert.value;

		if (r < 1.0)
		{
			lightcos = (r - lightcos) / (r + 1.0);

			if (lightcos > 0.0)
				illum += r_shadelight * lightcos;
		}
		else
		{
			illum += r_shadelight;
			lightcos = (lightcos + (r - 1.0)) / r; 		// do modified hemispherical lighting

			if (lightcos > 0.0)
				illum -= r_shadelight * lightcos;
		}

		if (illum <= 0)
			illum = 0;
	}

	if (illum > 255)
		illum = 255;

#if defined( GLQUAKE )
	*lv = lightgammatable[(int)illum * 4] / 1023.0;	// Light from 0 to 1.0
#else
	*lv = lightgammatable[(int)illum * 4] * 64.0;	// Software lighting
#endif
}

/*
================
R_LightStrength

================
*/
void R_LightStrength( int bone, float* vert, float(*light)[4] )
{
	int	i;

	if (lightage[bone] != r_smodels_total)
	{
		for (i = 0; i < numlights; i++)
		{
			vec3_t lpos;
			lpos[0] = locallight[i]->origin[0] - lighttransform[bone][0][3];
			lpos[1] = locallight[i]->origin[1] - lighttransform[bone][1][3];
			lpos[2] = locallight[i]->origin[2] - lighttransform[bone][2][3];
			VectorIRotate(lpos, lighttransform[bone], lightbonepos[bone][i]);
		}

		lightage[bone] = r_smodels_total;
	}

	for (i = 0; i < numlights; i++)
	{
		VectorSubtract(vert, lightbonepos[bone][i], light[i]);
		light[i][3] = 0.0;
	}
}

/*
====================
R_LightLambert

Lambert studio lighting

Designed to prevent the rear of a studio model losing its shape and looking too flat
The Lambert lighting is completely non-physical, it gives a purely percieved visual
enhancement and is an example of a forgiving lighting model
====================
*/
#if !defined( GLQUAKE )
// Software Lambert lighting, use a mixed color
void R_LightLambert( float(*light)[4], float* normal, int* lambert )
{
	// TODO: Implement
}
#else
// GL Lambert lighting
void R_LightLambert( float(*light)[4], float* normal, float* src, float* lambert )
{
	int		i;
	float	adjr, adjg, adjb;
	float	c;
	int		j;

	adjr = 0.0;
	adjg = 0.0;
	adjb = 0.0;

	for (i = 0; i < numlights; i++)
	{
		float r2, r;

		r = -DotProduct(normal, light[i]);		
		if (r > 0.0)
		{
			if (light[i][3] == 0.0)
			{
				r2 = DotProduct(light[i], light[i]);
				if (r2 > 0.0)
					light[i][3] = locallightR2[i] / (r2 * sqrt(r2));
				else
					light[i][3] = 1.0;
			}

			c = r * light[i][3];
			adjr += locallinearlight[i][0] * c;
			adjg += locallinearlight[i][1] * c;
			adjb += locallinearlight[i][2] * c;
		}
	}

	// No light at all
	if (adjr == 0.0 && adjg == 0.0 && adjb == 0.0)
	{
		lambert[0] = src[0];
		lambert[1] = src[1];
		lambert[2] = src[2];
		return;
	}

	//
	// Apply light effect
	//
	j = adjr + lineargammatable[(int)(src[0] * 1023.0)];
	if (j > 1023)
		lambert[0] = 1.0;	
	else
		lambert[0] = screengammatable[j] / 1023.0;

	j = adjg + lineargammatable[(int)(src[1] * 1023.0)];
	if (j > 1023)
		lambert[1] = 1.0;
	else
		lambert[1] = screengammatable[j] / 1023.0;

	j = adjb + lineargammatable[(int)(src[2] * 1023.0)];
	if (j > 1023)
		lambert[2] = 1.0;
	else
		lambert[2] = screengammatable[j] / 1023.0;
}
#endif

/*
================
R_StudioChrome

================
*/
void R_StudioChrome( int* pchrome, int bone, vec_t* normal )
{
	float n;

	if (chromeage[bone] != r_smodels_total)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		vec3_t chromeupvec;		// chrome t vector in world reference frame
		vec3_t chromerightvec;	// chrome s vector in world reference frame
		vec3_t tmp;				// vector pointing at bone in world reference frame

		VectorScale(r_origin, -1.0, tmp);

		tmp[0] += lighttransform[bone][0][3];
		tmp[1] += lighttransform[bone][1][3];
		tmp[2] += lighttransform[bone][2][3];

		VectorNormalize(tmp);
		CrossProduct(tmp, vright, chromeupvec);
		VectorNormalize(chromeupvec);
		CrossProduct(chromeupvec, tmp, chromerightvec);
		VectorNormalize(chromerightvec);

		VectorIRotate(chromeupvec, lighttransform[bone], r_chromeup[bone]);
		VectorIRotate(chromerightvec, lighttransform[bone], r_chromeright[bone]);

		chromeage[bone] = r_smodels_total;
	}

	// calc s coord
	n = DotProduct(normal, r_chromeright[bone]);
#if defined( GLQUAKE )
	pchrome[0] = (n + 1.0) * (32.0 * 1024.0);
#else
	pchrome[0] = (n + 1.0) * (2.0 * 1024.0 * 1024.0);
#endif

	// calc t coord
	n = DotProduct(normal, r_chromeup[bone]);
#if defined( GLQUAKE )
	pchrome[1] = (n + 1.0) * (32.0 * 1024.0);
#else
	pchrome[1] = (n + 1.0) * (2.0 * 1024.0 * 1024.0);
#endif
}

/*
===========
R_StudioSetupLighting
Applies lighting effects to model
set some global variables based on entity position
inputs:
outputs:
	r_ambientlight
	r_shadelight
===========
*/
void R_StudioSetupLighting( alight_t* plighting )
{
	int i;

	r_ambientlight = plighting->ambientlight;
	r_shadelight = plighting->shadelight;

	VectorCopy(plighting->plightvec, r_plightvec);

	for (i = 0; i < pstudiohdr->numbones; i++)
	{
		VectorIRotate(r_plightvec, lighttransform[i], r_blightvec[i]);
	}

	// the colorVec acceps 0-FFFFF range of colors, scale it with 192 and 255 respectively (C0 and FF)
	r_icolormix.r = (int)(plighting->color[0] * 0xC0FF) & 0xFF00;
	r_icolormix.g = (int)(plighting->color[1] * 0xC0FF) & 0xFF00;
	r_icolormix.b = (int)(plighting->color[2] * 0xC0FF) & 0xFF00;

	r_colormix[0] = plighting->color[0];
	r_colormix[1] = plighting->color[1];
	r_colormix[2] = plighting->color[2];
}

/*
====================
R_StudioSetupModel

Based on the body part, figure out which mesh it should be using
====================
*/
void R_StudioSetupModel( int bodypart )
{
	int index;

	if (bodypart > pstudiohdr->numbodyparts)
	{
		Con_DPrintf("R_StudioSetupModel: no such bodypart %d\n", bodypart);
		bodypart = 0;
	}
	
	pbodypart = (mstudiobodyparts_t*)((byte*)pstudiohdr + pstudiohdr->bodypartindex) + bodypart;

	index = currententity->body / pbodypart->base;
	index = index % pbodypart->nummodels;

	psubmodel = (mstudiomodel_t*)((byte*)pstudiohdr + pbodypart->modelindex) + index;
}

/*
====================
R_StudioDrawModel

====================
*/
int R_StudioDrawModel( int flags )
{
	alight_t lighting;
	vec3_t dir;

	VectorCopy(currententity->origin, r_entorigin);
	VectorSubtract(r_origin, r_entorigin, modelorg);

	pstudiohdr = (studiohdr_t*)Mod_Extradata(currententity->model);

	R_StudioSetUpTransform(0);

	if (flags & STUDIO_RENDER)
	{
		// see if the bounding box lets us trivially reject, also sets
		if (!R_StudioCheckBBox())
			return 0;

		r_amodels_drawn++;
		r_smodels_total++; // render data cache cookie

		if (pstudiohdr->numbodyparts == 0)
			return 1;
	}

	R_StudioSetupBones();

	if (flags & STUDIO_EVENTS)
	{
		R_StudioClientEvents();

		// copy attachments into global entity array
		if (currententity->index > 0)
			memcpy(cl_entities[currententity->index].attachment, currententity->attachment, sizeof(vec3_t) * 4);
	}

	if (flags & STUDIO_RENDER)
	{
		lighting.plightvec = dir;
		R_StudioDynamicLight(currententity, &lighting);

		R_StudioEntityLight(&lighting);

		// model and frame independant
		R_StudioSetupLighting(&lighting);

		R_StudioRenderFinal();
	}

	return 1;
}

/*
====================
R_StudioDrawPlayer

====================
*/
int R_StudioDrawPlayer( int flags, player_state_t* pplayer )
{
	alight_t lighting;
	vec3_t dir;

	VectorCopy(currententity->origin, r_entorigin);
	VectorSubtract(r_origin, r_entorigin, modelorg);

	pstudiohdr = (studiohdr_t*)Mod_Extradata(currententity->model);

	R_StudioSetUpTransform(0);

	if (flags & STUDIO_RENDER)
	{
		// see if the bounding box lets us trivially reject, also sets
		if (!R_StudioCheckBBox())
			return 0;

		r_amodels_drawn++;
		r_smodels_total++; // render data cache cookie

		if (pstudiohdr->numbodyparts == 0)
			return 1;
	}

	R_StudioSetupBones();

	if (flags & STUDIO_EVENTS)
	{
		R_StudioClientEvents();

		// copy attachments into global entity array
		if (currententity->index > 0)
			memcpy(cl_entities[currententity->index].attachment, currententity->attachment, sizeof(vec3_t) * 4);
	}

	if (flags & STUDIO_RENDER)
	{
		lighting.plightvec = dir;
		R_StudioDynamicLight(currententity, &lighting);

		R_StudioEntityLight(&lighting);

		// model and frame independant
		R_StudioSetupLighting(&lighting);

		R_StudioRenderFinal();

		if (pplayer->weaponmodel)
		{
			studiohdr_t* saveheader;
			model_t* pweaponmodel;

			saveheader = pstudiohdr;
			pweaponmodel = cl.model_precache[pplayer->weaponmodel];

			pstudiohdr = (studiohdr_t*)Mod_Extradata(pweaponmodel);

			R_StudioMergeBones(saveheader, pweaponmodel);

			R_StudioRenderFinal();
		}
	}

	return 1;
}

/*
====================
R_StudioDynamicLight

Apply lighting effects to a model
====================
*/
void R_StudioDynamicLight( cl_entity_t* ent, alight_t* plight )
{
	int			lnum;
	vec3_t		dist; // distance between dlight and entity origin
	colorVec	down;
	vec3_t		light;
	float		total;
	float		r, add;
	vec3_t		uporigin, upend;
	float		floor;
	vec3_t		color;

	// fullbright mode, set max brightness and go away
	if (r_fullbright.value == 1.0)
	{
		plight->shadelight = 0;
		plight->ambientlight = 192;
		plight->plightvec[0] = 0.0;
		plight->plightvec[1] = 0.0;
		plight->plightvec[2] = -1.0;
		plight->color[0] = 1.0;
		plight->color[1] = 1.0;
		plight->color[2] = 1.0;
		return;
	}

	//
	// setup ambient lighting
	light[1] = 0.0;
	light[0] = 0.0;
	light[2] = 1.0;

	if (!(ent->effects & EF_INVLIGHT))
		light[2] = -1.0;

	VectorCopy(ent->origin, uporigin);
	uporigin[2] -= light[2] * 8.0;

	down.r = down.g = down.b = down.a = 0;

	// check sky color values
	if ((cl_skycolor_r.value + cl_skycolor_g.value + cl_skycolor_b.value) != 0)
	{
		vec3_t end;
		msurface_t* psurf;

		end[0] = ent->origin[0] - cl_skyvec_x.value * 8192.0;
		end[1] = ent->origin[1] - cl_skyvec_y.value * 8192.0;
		end[2] = ent->origin[2] - cl_skyvec_z.value * 8192.0;

		psurf = SurfaceAtPoint(cl.worldmodel, cl.worldmodel->nodes, uporigin, end);
		if ((ent->model->flags & STUDIO_FORCE_SKYLIGHT) || (psurf && (psurf->flags & SURF_DRAWSKY)))
		{
			down.r = cl_skycolor_r.value;
			down.g = cl_skycolor_g.value;
			down.b = cl_skycolor_b.value;
			light[0] = cl_skyvec_x.value;
			light[1] = cl_skyvec_y.value;
			light[2] = cl_skyvec_z.value;
		}
	}

	// see if the model is not illuminated by the sky 
	if ((down.r + down.g + down.b) == 0)
	{
		colorVec gcolor;
		float grad[4];

		VectorScale(light, 2048.0, upend);
		VectorAdd(upend, uporigin, upend);

		down = R_LightVec(uporigin, upend);

		uporigin[0] -= 16.0;
		uporigin[1] -= 16.0;
		upend[0] -= 16.0;
		upend[1] -= 16.0;
		gcolor = R_LightVec(uporigin, upend);
		grad[0] = (float)(gcolor.r + gcolor.g + gcolor.b) / 768.0;

		uporigin[0] += 32.0;
		upend[0] += 32.0;
		gcolor = R_LightVec(uporigin, upend);
		grad[1] = (float)(gcolor.r + gcolor.g + gcolor.b) / 768.0;

		uporigin[1] += 32.0;
		upend[1] += 32.0;
		gcolor = R_LightVec(uporigin, upend);
		grad[2] = (float)(gcolor.r + gcolor.g + gcolor.b) / 768.0;

		uporigin[0] -= 32.0;
		upend[0] -= 32.0;
		gcolor = R_LightVec(uporigin, upend);
		grad[3] = (float)(gcolor.r + gcolor.g + gcolor.b) / 768.0;

		// calc light direction
		light[0] = grad[0] - grad[1] - grad[2] + grad[3];
		light[1] = grad[0] + grad[1] - grad[2] - grad[3];
		VectorNormalize(light);
	}

	// Set floor light
	currententity->cvFloorColor.r = down.r;
	currententity->cvFloorColor.g = down.g;
	currententity->cvFloorColor.b = down.b;
	currententity->cvFloorColor.a = down.a;

	color[0] = down.r;
	color[1] = down.g;
	color[2] = down.b;

	// intentsity
	floor = max(max(color[0], color[1]), color[2]);
	if (floor == 0.0)
		floor = 1.0;

	VectorScale(light, floor, light);

	//
	// add dynamic lights

	for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		dlight_t* dl;

		dl = &cl_dlights[lnum];

		// it's dead already, so just skip
		if (dl->die < cl.time)
			continue;

		VectorSubtract(ent->origin, dl->origin, dist);

		r = Length(dist);
		add = (dl->radius - r); // squared radius
		if (add > 0.0)
		{
			floor += add;

			if (r > 1.0)
				VectorScale(dist, add / r, dist);
			else
				VectorScale(dist, add, dist);

			VectorAdd(light, dist, light);

			color[0] += dl->color.r * (add / 256.0);
			color[1] += dl->color.g * (add / 256.0);
			color[2] += dl->color.b * (add / 256.0);
		}
	}

	if (ent->model->flags & STUDIO_DYNAMIC_LIGHT)
		total = 0.6;
	else
		total = v_direct.value;

	VectorScale(light, total, light);

	plight->shadelight = Length(light);
	plight->ambientlight = (floor - plight->shadelight);

	floor = max(max(color[0], color[1]), color[2]);
	if (floor == 0.0)
	{
		plight->color[0] = 1.0;
		plight->color[1] = 1.0;
		plight->color[2] = 1.0;
	}
	else
	{
		plight->color[0] = color[0] * (1.0 / floor);
		plight->color[1] = color[1] * (1.0 / floor);
		plight->color[2] = color[2] * (1.0 / floor);
	}

	// clamp the lighting, so it doesn't "overbright" too much
	if (plight->ambientlight > 128)
		plight->ambientlight = 128;

	if ((plight->ambientlight + plight->shadelight) > 255)
		plight->shadelight = 255 - plight->ambientlight;

	VectorNormalize(light);
	VectorCopy(light, plight->plightvec);
}

// Apply entity lighting
void R_StudioEntityLight( alight_t* plight )
{
	int		i, j, k;
	dlight_t* el;
	vec3_t	mid, pos;
	float	dist2, f;
	float	radius;
	float	lstrength[MAXLOCALLIGHTS];
	float	minstrength;

	VectorCopy(currententity->origin, pos);

	lstrength[0] = 0.0;
	lstrength[1] = 0.0;
	lstrength[2] = 0.0;

	minstrength = 1000000.0;

	j = 0;
	numlights = 0; // clear previous elights

	for (i = 0; i < MAX_ELIGHTS; i++)
	{
		el = &cl_elights[i];

		if (el->die <= cl.time || el->radius <= 0.0)
			continue;
		
		// Beam entities
		if (BEAMENT_ENTITY(el->key) == currententity->index)
		{
			if (BEAMENT_ATTACHMENT(el->key))
			{
				VectorCopy(currententity->attachment[BEAMENT_ATTACHMENT(el->key)], el->origin);
			}
			else
			{
				VectorCopy(currententity->origin, el->origin);
			}
		}

		VectorSubtract(pos, el->origin, mid);

		f = DotProduct(mid, mid);
		radius = el->radius * el->radius; // squared radius

		if (f > radius)
			dist2 = (radius / f);
		else
			dist2 = 1.0;

		if (dist2 > 0.004)
		{
			int att;
			if (j < 3)
			{
				att = j;
			}
			else
			{
				att = -1;
				for (k = 0; k < j; k++)
				{
					if (lstrength[k] < minstrength && lstrength[k] < dist2)
					{
						att = k;
						minstrength = lstrength[k];
					}
				}
			}

			if (att != -1)
			{
				numlights = j;
				lstrength[att] = dist2;
				locallight[att] = el;
				locallightR2[att] = radius;

				locallinearlight[att][0] = lineargammatable[el->color.r * 4];
				locallinearlight[att][1] = lineargammatable[el->color.g * 4];
				locallinearlight[att][2] = lineargammatable[el->color.b * 4];

				if (j <= att)
					j = att + 1;
			}
		}
	}

	numlights = j;
}

/*
=========================
R_StudioClientEvents

The entity's studio model description indicated an event was
fired during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
=========================
*/
void R_StudioClientEvents( void )
{
	int				i;
	mstudioevent_t* pevent;
	mstudioseqdesc_t* pseqdesc;
	mstudioattachment_t* pattachment;
	float			flStart, flEnd;
	static float	flCurTime = 0.0, flLastTime = 0.0;

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

	// calculate attachment points
	pattachment = (mstudioattachment_t*)((byte*)pstudiohdr + pstudiohdr->attachmentindex);
	for (i = 0; i < pstudiohdr->numattachments; i++)
	{
		VectorTransform(pattachment[i].org, lighttransform[pattachment[i].bone], currententity->attachment[i]);
	}

	for (; i < 4; i++)
	{
		VectorCopy(currententity->origin, currententity->attachment[i]);
	}

	if (cl.time == cl.oldtime)
		return;

	if (flCurTime != cl.time)
	{
		flLastTime = flCurTime;
		flCurTime = cl.time;
	}

	// EF_MUZZLEFLASH - do simple small light effect on attachment 0
	if (currententity->effects & EF_MUZZLEFLASH)
	{
		dlight_t* el;

		el = CL_AllocElight(0);
		VectorCopy(currententity->attachment[0], el->origin);
		el->radius = 16;
		el->decay = 320;
		el->color.r = 255;
		el->color.g = 192;
		el->color.b = 64;
		el->die = cl.time + 0.05; // die in a couple of frames

		currententity->effects &= ~EF_MUZZLEFLASH;
	}

	if (pseqdesc->numevents)
	{
		pevent = (mstudioevent_t*)((byte*)pstudiohdr + pseqdesc->eventindex);

		flEnd = CL_StudioEstimateFrame(pseqdesc);
		flStart = flEnd - (flCurTime - flLastTime) * pseqdesc->fps * currententity->framerate;

		if (currententity->sequencetime == currententity->animtime)
		{
			if (!(pseqdesc->flags & STUDIO_LOOPING))
				flStart = -0.01;
		}

		// Fire events
		for (i = 0; i < pseqdesc->numevents; i++, pevent++)
		{
			// client event numbers start at 5000 (e.g. 5001 is R_MuzzleFlash on entity attachment 0)
			if (pevent->event < 5000)
				continue;
			if (pevent->frame <= flStart || pevent->frame > flEnd)
				continue;

			switch (pevent->event)
			{
			case 5001:
				R_MuzzleFlash(currententity->attachment[0], lighttransform[pattachment->bone], atoi(pevent->options));
				break;
			case 5002:
				R_SparkEffect(currententity->attachment[0], atoi(pevent->options), -100, 100);
				break;
			default:
				break;
			}
		}
	}
}

#if !defined( GLQUAKE )
// TODO: Implement

/*
================
R_StudioRenderFinal

Finilize studio model rendering
================
*/
void R_StudioRenderFinal( void )
{
	// TODO: Implement
}
#endif

vec3_t* pvlightvalues;

#if defined( GLQUAKE )
/*
================
R_StudioRenderFinal

Finilize studio model rendering
================
*/
void R_StudioRenderFinal( void )
{
	int		i;
	float	an = 0.0;

	pauxverts = auxverts;
	pvlightvalues = lightvalues;

	shadevector[0] = cos(an);
	shadevector[1] = sin(an);
	shadevector[2] = 1.0;
	VectorNormalize(shadevector);

	GL_DisableMultitexture();
	qglPushMatrix();

	if (gl_smoothmodels.value)
		qglShadeModel(GL_SMOOTH);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	if (r_drawentities.value == 2)
	{
		R_StudioDrawBones();
	}
	else if (r_drawentities.value == 3)
	{
		R_StudioDrawHulls();
	}
	else if (r_drawentities.value == 4)
	{
		tri_GL_RenderMode(kRenderTransAdd);
		R_StudioDrawHulls();
		tri_GL_RenderMode(kRenderNormal);
	}
	else
	{
		for (i = 0; i < pstudiohdr->numbodyparts; i++)
		{
			R_StudioSetupModel(i);

			if (r_dointerp)
			{
				// interpolation messes up bounding boxes
				currententity->trivial_accept = 0;
			}

			if (currententity->rendermode != kRenderNormal)
			{
				if (currententity->rendermode == kRenderTransColor)
				{
					qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					qglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
				}
				else if (currententity->rendermode == kRenderTransAdd)
				{
					qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					qglBlendFunc(GL_ONE, GL_ONE);
					qglColor4f(r_blend, r_blend, r_blend, 1);
					qglDepthMask(GL_FALSE);
				}
				else
				{
					qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					qglColor4f(1, 1, 1, r_blend);
					qglDepthMask(GL_TRUE);
				}

				qglEnable(GL_BLEND);
			}

			R_StudioDrawPoints();

			qglDepthMask(GL_TRUE);

			if (r_shadows.value && currententity->rendermode != kRenderTransAdd)
			{
				if (!(currententity->model->flags & STUDIO_DYNAMIC_LIGHT))
				{
					float color;

					color = 1.0 - (r_blend * 0.5);

					qglDisable(GL_TEXTURE_2D);
					qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					qglEnable(GL_BLEND);

					qglColor4f(0, 0, 0, 1.0 - color);

					if (!gl_ztrick.value || gldepthmin < 0.5)
						qglDepthFunc(GL_LESS);
					else
						qglDepthFunc(GL_GREATER);

					GLR_StudioDrawShadow();

					if (!gl_ztrick.value || gldepthmin < 0.5)
						qglDepthFunc(GL_LEQUAL);
					else
						qglDepthFunc(GL_GEQUAL);

					qglEnable(GL_TEXTURE_2D);

					qglDisable(GL_BLEND);

					qglColor4f(1, 1, 1, 1);

					if (gl_smoothmodels.value)
						qglShadeModel(GL_SMOOTH);
				}
			}
		}
	}

	if (currententity->rendermode != kRenderNormal)
		qglDisable(GL_BLEND);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	qglShadeModel(GL_FLAT);

	if (gl_affinemodels.value)
		qglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	qglPopMatrix();
}

/*
================
R_StudioDrawPoints

================
*/
void R_StudioDrawPoints( void )
{
	int					i, j;
	byte* pvertbone;
	byte* pnormbone;
	vec3_t* pstudioverts;
	vec3_t* pstudionorms;
	mstudiotexture_t* ptexture;
	auxvert_t* av;
	float* lv;
	vec3_t				fl;
	float				lv_tmp;
	short* pskinref;
	int					flags;

	pvertbone = ((byte*)pstudiohdr + psubmodel->vertinfoindex);
	pnormbone = ((byte*)pstudiohdr + psubmodel->norminfoindex);
	ptexture = (mstudiotexture_t*)((byte*)pstudiohdr + pstudiohdr->textureindex);

	pmesh = (mstudiomesh_t*)((byte*)pstudiohdr + psubmodel->meshindex);

	pstudioverts = (vec3_t*)((byte*)pstudiohdr + psubmodel->vertindex);
	pstudionorms = (vec3_t*)((byte*)pstudiohdr + psubmodel->normindex);

	pskinref = (short*)((byte*)pstudiohdr + pstudiohdr->skinindex);
	if (currententity->skin != 0 && currententity->skin < pstudiohdr->numskinfamilies)
		pskinref += (currententity->skin * pstudiohdr->numskinref);

	for (i = 0; i < psubmodel->numverts; i++)
	{
		av = &pauxverts[i];
		R_StudioTransformAuxVert(av, pvertbone[i], pstudioverts[i]);
	}

	pstudioverts = (vec3_t*)((byte*)pstudiohdr + psubmodel->vertindex);
	pvertbone = ((byte*)pstudiohdr + psubmodel->vertinfoindex);
	for (i = 0; i < psubmodel->numverts; i++)
	{
		R_LightStrength(pvertbone[i], pstudioverts[i], lightpos[i]);
	}

//
// clip and draw all triangles
//
	lv = (float*)pvlightvalues;
	for (j = 0; j < psubmodel->nummesh; j++)
	{
		int k;
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;
		if (r_fullbright.value >= 2)
			flags &= ~(STUDIO_NF_FLATSHADE | STUDIO_NF_CHROME);

		if (currententity->rendermode == kRenderTransAdd)
		{
			for (k = 0; k < pmesh[j].numnorms; k++, lv += 3)
			{
				for (k = 0; k < pmesh[j].numnorms; k++, lv += 3)
				{
					lv[0] = r_blend;
					lv[1] = r_blend;
					lv[2] = r_blend;

					if (flags & STUDIO_NF_CHROME)
						R_StudioChrome(chrome[((byte*)lv - (byte*)pvlightvalues) / sizeof(vec3_t)], *pnormbone, (float*)pstudionorms);
				}
			}
		}
		else
		{
			for (k = 0; k < pmesh[j].numnorms; k++, lv += 3, pstudionorms++, pnormbone++)
			{
				R_StudioLighting(&lv_tmp, *pnormbone, flags, (float*)pstudionorms);

				if (flags & STUDIO_NF_CHROME)
					R_StudioChrome(chrome[((byte*)lv - (byte*)pvlightvalues) / sizeof(vec3_t)], *pnormbone, (float*)pstudionorms);

				lv[0] = lv_tmp * r_colormix[0];
				lv[1] = lv_tmp * r_colormix[1];
				lv[2] = lv_tmp * r_colormix[2];
			}
		}
	}

	qglCullFace(GL_FRONT);

	pstudionorms = (vec3_t*)((byte*)pstudiohdr + psubmodel->normindex);
	for (j = 0; j < psubmodel->nummesh; j++)
	{
		float s, t;
		short* ptricmds;

		pmesh = (mstudiomesh_t*)((byte*)pstudiohdr + psubmodel->meshindex) + j;
		ptricmds = (short*)((byte*)pstudiohdr + pmesh->triindex);

		c_alias_polys += pmesh->numtris;


		flags = ptexture[pskinref[pmesh->skinref]].flags;
		if (r_fullbright.value >= 2)
		{
			flags &= ~(STUDIO_NF_FLATSHADE | STUDIO_NF_CHROME);

			R_TriangleSpriteTexture(cl_sprite_white, 0);

			s = 1.0 / 256;
			t = 1.0 / 256;
		}
		else
		{
			s = 1.0 / ptexture[pskinref[pmesh->skinref]].width;
			t = 1.0 / ptexture[pskinref[pmesh->skinref]].height;

			GL_Bind(ptexture[pskinref[pmesh->skinref]].index);
		}

		if (flags & STUDIO_NF_CHROME)
		{
			s *= 1.0 / 1024;
			t *= 1.0 / 1024;

			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					qglBegin(GL_TRIANGLE_FAN);
					i = -i;
				}
				else
				{
					qglBegin(GL_TRIANGLE_STRIP);
				}

				for (; i > 0; i--, ptricmds += 4)
				{
					qglTexCoord2f(chrome[ptricmds[1]][0] * s, chrome[ptricmds[1]][1] * t);

					lv = pvlightvalues[ptricmds[1]];
					R_LightLambert(lightpos[ptricmds[0]], pstudionorms[ptricmds[1]], lv, fl);
					qglColor4f(fl[0], fl[1], fl[2], r_blend);

					av = &pauxverts[ptricmds[0]];
					qglVertex3f(av->fv[0], av->fv[1], av->fv[2]);
				}

				qglEnd();
			}
		}
		else
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					qglBegin(GL_TRIANGLE_FAN);
					i = -i;
				}
				else
				{
					qglBegin(GL_TRIANGLE_STRIP);
				}

				for (; i > 0; i--, ptricmds += 4)
				{
					qglTexCoord2f(ptricmds[2] * s, ptricmds[3] * t);

					lv = pvlightvalues[ptricmds[1]];
					R_LightLambert(lightpos[ptricmds[0]], pstudionorms[ptricmds[1]], lv, fl);
					qglColor4f(fl[0], fl[1], fl[2], r_blend);

					av = &pauxverts[ptricmds[0]];
					qglVertex3f(av->fv[0], av->fv[1], av->fv[2]);
				}

				qglEnd();
			}
		}
	}
}

void GLR_StudioDrawShadow( void )
{
	// TODO: Implement
}
#endif

// TODO: Implement