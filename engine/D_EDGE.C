// d_edge.c

#include "quakedef.h"
#include "d_local.h"

static int	miplevel;

float		scale_for_mip;

int			ubasestep, errorterm, erroradjustup, erroradjustdown;