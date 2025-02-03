// pr_cmds.h
#ifndef PR_CMDS_H
#define PR_CMDS_H
#ifdef _WIN32
#pragma once
#endif

float	RandomFloat( float flLow, float flHigh );
int32	RandomLong( int32 lLow, int32 lHigh );

// TODO: Implement

struct mnode_s* PVSNode( struct mnode_s* node, vec_t* emins, vec_t* emaxs );

#endif // PR_CMDS_H