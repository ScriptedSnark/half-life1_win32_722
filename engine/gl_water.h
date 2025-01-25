// gl_water.h
#ifndef GL_WATER_H
#define GL_WATER_H
#ifdef _WIN32
#pragma once
#endif

void GL_SubdivideSurface( msurface_t* fa );
void EmitWaterPolys( msurface_t* fa, int direction );
void R_DrawSkyChain( msurface_t* s );

extern float turbsin[];

#endif // GL_WATER_H