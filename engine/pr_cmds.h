// pr_cmds.h
#ifndef PR_CMDS_H
#define PR_CMDS_H
#ifdef _WIN32
#pragma once
#endif

msurface_t* SurfaceAtPoint( model_t* pModel, mnode_t* node, vec_t* start, vec_t* end );

struct mnode_s* PVSNode( struct mnode_s* node, vec_t* emins, vec_t* emaxs );

float	RandomFloat( float flLow, float flHigh );
int32	RandomLong( int32 lLow, int32 lHigh );

#endif // PR_CMDS_H