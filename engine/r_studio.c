// r_studio.c: routines for setting up to draw 3DStudio models 

#include "quakedef.h"
#include "r_studio.h"

// Pointer to header block for studio model data
studiohdr_t* pstudiohdr;

// TODO: Implement

// Model to world transformation
float			rotationmatrix[3][4];

// Concatenated bone and light transforms
float			bonetransform[MAXSTUDIOBONES][3][4];
float			lighttransform[MAXSTUDIOBONES][3][4];

// TODO: Implement

// Do interpolation?
int r_dointerp = 1;


// TODO: Implement



// TODO: Implement

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

// TODO: Implement

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

// TODO: Implement

/*
====================
R_StudioCalcBoneAdj

Compute bone adjustments ( bone controllers )
====================
*/
void R_StudioCalcBoneAdj( float dadt, float* adj, const unsigned char* pcontroller1, const unsigned char* pcontroller2, unsigned char mouthopen )
{
	// TODO: Implement
}

void R_StudioCalcBoneQuaterion( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q )
{
	// TODO: Implement
}

void R_StudioCalcBonePosition( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos )
{
	// TODO: Implement
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
	// TODO: Implement
}

/*
====================
R_GetAnim

====================
*/
mstudioanim_t* R_GetAnim( model_t* psubmodel, mstudioseqdesc_t* pseqdesc )
{
	// TODO: Implement
	return NULL;
}

/*
====================
R_StudioSlerpBones

====================
*/
void R_StudioSlerpBones( vec4_t* q1, vec3_t* pos1, vec4_t* q2, vec3_t* pos2, float s )
{
	// TODO: Implement
}

float CL_StudioEstimateFrame( mstudioseqdesc_t* pseqdesc )
{
	// TODO: Implement
	return 0.0;
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

// TODO: Implement

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
	// TODO: Implement
}

// TODO: Implement

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
	// TODO: Implement
	return 1;
}

// Apply lighting effects to a model
void R_StudioDynamicLight( cl_entity_t* ent, alight_t* plight )
{
	// TODO: Implement
}

// Apply entity lighting
void R_StudioEntityLight( alight_t* plight )
{
	// TODO: Implement
}

// TODO: Implement

void R_StudioClientEvents( void )
{
	// TODO: Implement
}

// TODO: Implement

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
int			pvlightvalues_size;

#if defined( GLQUAKE )
void GLR_StudioDrawShadow( void );

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

// TODO: Implement
#endif

// TODO: Implement