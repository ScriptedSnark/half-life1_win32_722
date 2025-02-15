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
	// TODO: Implement
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