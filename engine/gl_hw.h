#ifndef GL_HW_H
#define GL_HW_H
#pragma once

enum
{
	GL_HW_UNKNOWN = 0,
	GL_HW_3Dfx,
	GL_HW_RIVA128,
	GL_HW_PCX2,
	GL_HW_PVRSG,
	GL_HW_RENDITIONV2200,
	GL_HW_3DLABS,
	GL_HW_ATIRAGE,
	GL_HW_i740,
	GL_HW_G200,
	GL_HW_RIVATNT,
	GL_HW_GEFORCE,
};

extern int gGLHardwareType;

#endif // GL_HW_H