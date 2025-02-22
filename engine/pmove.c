#include "quakedef.h"
#include "pmove.h"

movevars_t		movevars;

playermove_t	pmove;

int			onground;
int			waterlevel;
int			watertype;

float		frametime;

vec3_t		forward, right, up;

vec3_t	player_mins[4] = {{-16, -16, -36}, {-16, -16, -18}, {0, 0, 0}, 0};
vec3_t	player_maxs[4] = {{16, 16, 36}, {16, 16, 18}, {0, 0, 0}, 0};