// r_edge.c

#include "quakedef.h"

#if 0
// FIXME
the complex cases add new polys on most lines, so dont optimize for keeping them the same
have multiple free span lists to try to get better coherence ?
low depth complexity-- 1 to 3 or so

this breaks spans at every edge, even hidden ones(bad)

have a sentinal at both ends ?
#endif


edge_t* auxedges;
edge_t* r_edges, * edge_p, * edge_max;

surf_t* surfaces, * surface_p, * surf_max;

// surfaces are generated in back to front order by the bsp, so if a surf
// pointer is greater than another one, it should be drawn in front
// surfaces[1] is the background, and is used as the active surface stack

edge_t* newedges[MAXHEIGHT];
edge_t* removeedges[MAXHEIGHT];

espan_t* span_p, * max_span_p;

int		r_currentkey;

extern	int	screenwidth;

int	current_iv;

int	edge_head_u_shift20, edge_tail_u_shift20;

static void (*pdrawfunc)( void );

edge_t	edge_head;
edge_t	edge_tail;
edge_t	edge_aftertail;
edge_t	edge_sentinel;

float	fv;


/*
==============
R_BeginEdgeFrame
==============
*/
void R_BeginEdgeFrame( void )
{
	// TODO: Implement
}


/*
==============
R_ScanEdges

Input:
newedges[] array
	this has links to edges, which have links to surfaces

Output:
Each surface has a linked list of its visible spans
==============
*/
void R_ScanEdges( void )
{
	// TODO: Implement
}