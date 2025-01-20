//
// spritegn.h: header file for sprite generation program
//

#ifndef SPRITEGN_H
#define SPRITEGN_H

#ifdef _WIN32
#pragma once
#endif

// **********************************************************
// * This file must be identical in the spritegen directory *
// * and in the Quake directory, because it's used to       *
// * pass data from one to the other via .spr files.        *
// **********************************************************

//-------------------------------------------------------
// This program generates .spr sprite package files.
// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>
//-------------------------------------------------------

#ifdef INCLUDELIBS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "cmdlib.h"
#include "scriplib.h"
#include "lbmlib.h"

#endif

#define SPRITE_VERSION	2

// MAX_SPRITES limit
#define MAX_SPRITES		256

// must match definition in modelgen.h
#ifndef SYNCTYPE_T
#define SYNCTYPE_T

typedef enum synctype_e
{
	ST_SYNC = 0,
	ST_RAND
} synctype_t;
#endif

typedef enum spriteframetype_e
{
	SPR_SINGLE = 0,
	SPR_GROUP
} spriteframetype_t;

// TODO: shorten these?
typedef struct dsprite_s
{
	int					ident;
	int					version;
	spriteframetype_t	type;
	int					texFormat;
	float				boundingradius;
	int					width;
	int					height;
	int					numframes;
	float				beamlength;
	synctype_t			synctype;
} dsprite_t;

#define SPR_VP_PARALLEL_UPRIGHT		0
#define SPR_FACING_UPRIGHT			1
#define SPR_VP_PARALLEL				2
#define SPR_ORIENTED				3
#define SPR_VP_PARALLEL_ORIENTED	4

#define SPR_NORMAL					0
#define SPR_ADDITIVE				1
#define SPR_INDEXALPHA				2
#define SPR_ALPHTEST				3

typedef struct dspriteframe_s
{
	int			origin[2];
	int			width;
	int			height;
} dspriteframe_t;

typedef struct dspritegroup_s
{
	int			numframes;
} dspritegroup_t;

typedef struct dspriteinterval_s
{
	float	interval;
} dspriteinterval_t;

typedef struct dspriteframetype_s
{
	spriteframetype_t	type;
} dspriteframetype_t;

#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I')
														// little-endian "IDSP"

#endif // SPRITEGN_H