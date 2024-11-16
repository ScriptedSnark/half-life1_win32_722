#ifndef R_SHARED_H
#define R_SHARED_H
#pragma once

#ifndef GLQUAKE
// r_shared.h: general refresh-related stuff shared between the refresh and the
// driver

typedef byte pixel_t;


typedef struct
{
	char			name[64];
	cache_user_t	cache;
} cachepic_t;






#endif // GLQUAKE

#endif // R_SHARED_H