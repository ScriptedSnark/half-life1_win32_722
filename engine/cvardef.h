#ifndef CVARDEF_H
#define CVARDEF_H

typedef struct cvar_s
{
	char* name;
	char* string;
	int		archive;		// set to true to cause it to be saved to vars.rc
	int		server;			// notifies players when changed
	int		profile;
	float	value;
	struct cvar_s* next;
} cvar_t;
#endif