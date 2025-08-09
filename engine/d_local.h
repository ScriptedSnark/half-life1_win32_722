// d_local.h:  private rasterization driver defs
#if !defined( D_LOCAL_H )
#define D_LOCAL_H
#ifdef _WIN32
#pragma once
#endif

#include "r_shared.h"

#define SURFCACHE_SIZE_AT_320X200		1024 * 1024

extern short* d_pzbuffer;

#endif // D_LOCAL_H