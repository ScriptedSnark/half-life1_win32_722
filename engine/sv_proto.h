#ifndef SV_PROTO_H
#define SV_PROTO_H
#pragma once

hull_t* SV_HullForBsp( edict_t* ent, const vec_t* mins, const vec_t* maxs, vec_t* offset );
void SV_SetGlobalTrace( trace_t* ptrace );
trace_t SV_ClipMoveToEntity( edict_t* ent, const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end );

#endif // SV_PROTO_H