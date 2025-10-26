//
// util.cpp
//
// implementation of class-less helper functions
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hud.h"
#include "util.h"
#include <string.h>



HSPRITE_t LoadSprite(const char *pszName)
{
	int i;
	char sz[256]; 

	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;

	sprintf(sz, pszName, i);

	return SPR_Load(sz);
}

