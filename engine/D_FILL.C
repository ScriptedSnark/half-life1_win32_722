// d_clear: clears a specified rectangle to the specified color

#include "quakedef.h"


/*
================
D_FillRect
================
*/
void D_FillRect( vrect_t* rect, byte* color )
{
	int				rx, ry, rwidth, rheight;
	byte* dest;
	word			colorVal;

	if (is15bit)
		colorVal = PACKEDRGB555(color[0], color[1], color[2]);
	else
		colorVal = PACKEDRGB565(color[0], color[1], color[2]);

	rx = rect->x;
	ry = rect->y;
	rwidth = rect->width;
	rheight = rect->height;

	if (rx < 0)
	{
		rwidth += rx;
		rx = 0;
	}
	if (ry < 0)
	{
		rheight += ry;
		ry = 0;
	}
	if (rx + rwidth > (int)vid.width)
		rwidth = vid.width - rx;
	if (ry + rheight > (int)vid.height)
		rheight = vid.height - rx;

	if (rwidth < 1 || rheight < 1)
		return;

	dest = ((byte*)vid.buffer + ry * vid.rowbytes + rx * 2);

	if (((rwidth & 0x03) == 0) && (((long)dest & 0x03) == 0))
	{
	// faster aligned dword clear
		rwidth >>= 1;

		for (ry = 0; ry < rheight; ry++)
		{
			for (rx = 0; rx < rwidth; rx++)
				((int*)dest)[rx] = (int)colorVal | ((int)colorVal << 16);
			dest += vid.rowbytes;
		}
	}
	else
	{
	// slower byte-by-byte clear for unaligned cases
		for (ry = 0; ry < rheight; ry++)
		{
			for (rx = 0; rx < rwidth; rx++)
				((word*)dest)[rx] = colorVal;
			dest += vid.rowbytes;
		}
	}
}