// vmodes.h - header for videomodes
#ifndef VMODES_H
#define VMODES_H
#pragma once


typedef enum
{
	VT_None = 0,
	VT_Software,
	VT_OpenGL,
	VT_Direct3D,
} VidTypes;

// a pixel can be one, two, or four bytes
typedef byte pixel_t;

typedef struct vrect_s
{
	int				x, y, width, height;
	struct vrect_s* pnext;
} vrect_t;



#endif //VMODES_H