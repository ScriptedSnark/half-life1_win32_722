#include "quakedef.h"
#include "pmove.h"

/*
=================
PM_PlayerTrace

Perform a trace from a starting point to an ending point and determine what it intersects with
traceFlags - whether it should only check against the
world geometry or should ignore glass
=================
*/
pmtrace_t PM_PlayerTrace( vec_t* start, vec_t* end, int traceFlags )
{
	pmtrace_t trace/*, total*/;

	memset(&trace, 0, sizeof(trace));
	trace.fraction = 1.0;
	trace.ent = -1;
	VectorCopy(end, trace.endpos);

	// TODO: Implement

	return trace;
}