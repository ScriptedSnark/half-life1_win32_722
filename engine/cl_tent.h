// cl_tent.h
#ifndef CL_TENT_H
#define CL_TENT_H
#ifdef _WIN32
#pragma once
#endif

extern sfx_t* cl_sfx_geiger1;
extern sfx_t* cl_sfx_geiger2;
extern sfx_t* cl_sfx_geiger3;
extern sfx_t* cl_sfx_geiger4;
extern sfx_t* cl_sfx_geiger5;
extern sfx_t* cl_sfx_geiger6;

void CL_TempEntInit( void );

TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model );

void R_RicochetSprite( float* pos, model_t* pmodel, float duration, float scale );
void R_RocketFlare( float* pos );
void R_MuzzleFlash( float* pos1, float (*light)[4], int type );
void R_SparkEffect( float* pos, int count, int velocityMin, int velocityMax );

TEMPENTITY* R_TempModel( float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype );

#endif // CL_TENT_H