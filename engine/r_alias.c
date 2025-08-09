// r_alias.c: routines for setting up to draw alias models

#include "quakedef.h"

// TODO: these probably will go away with optimized rasterization
mdl_t* pmdl;
vec3_t				r_plightvec;
int					r_ambientlight;
float				r_shadelight;

int				r_amodels_drawn;

float	aliastransform[3][4];

#define NUMVERTEXNORMALS	162

float r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};