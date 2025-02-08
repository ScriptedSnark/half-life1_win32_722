// world.h
#ifndef WORLD_H
#define WORLD_H
#ifdef _WIN32
#pragma once
#endif

void SV_ClearWorld( void );
// called after the world model has been loaded, before linking any entities


qboolean SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, trace_t* trace );

#endif // WORLD_H