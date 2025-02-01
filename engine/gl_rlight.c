// gl_rlight.c

#include "quakedef.h"
#include "r_local.h"

int	r_dlightframecount;
int	r_dlightchanged;
int r_dlightactive;


/*
==================
R_AnimateLight
==================
*/
void R_AnimateLight( void )
{
	int			i, j, k;

//
// light animations
// 'm' is normal light, 'a' is no light, 'z' is double bright
	i = (int)(cl.time * 10);
	for (j = 0; j < MAX_LIGHTSTYLES; j++)
	{
		if (!cl_lightstyle[j].length)
		{
			d_lightstylevalue[j] = 256;
			continue;
		}
		k = i % cl_lightstyle[j].length;
		k = cl_lightstyle[j].map[k] - 'a';
		k = k * 22;
		d_lightstylevalue[j] = k;
	}
}

/*
=============================================================================

DYNAMIC LIGHTS BLEND RENDERING

=============================================================================
*/

// TODO: Implement

/*
=============
R_RenderDlights
=============
*/
void R_RenderDlights( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
=============
R_PushDlights
=============
*/
void R_PushDlights( void )
{
	// TODO: Implement
}

// TODO: Implement