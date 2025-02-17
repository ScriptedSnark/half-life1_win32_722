#include "quakedef.h"
#include "server.h"

#include "world.h"

edict_t* EDICT_NUM( int n )
{
	if (n < 0 ||
		n >= sv.max_edicts)
		Sys_Error("EDICT_NUM: bad number %i", n);
	return &sv.edicts[n];
}

int NUM_FOR_EDICT( const edict_t* e )
{
	int		b;

	b = e - sv.edicts;

	if (b < 0 || b >= sv.num_edicts)
		Sys_Error("NUM_FOR_EDICT: bad pointer");
	return b;
}
