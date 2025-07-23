// cl_servercache.h
#if !defined( CL_SERVERCACHE_H )
#define CL_SERVERCACHE_H
#ifdef _WIN32
#pragma once
#endif

// Server cache to replace slist command.
#define MAX_LOCAL_SERVERS 8

typedef struct
{
	char		name[80];
	char		desc[256];
	char		map[16];
	char		info[32];

	int			inuse;
	int			maxplayers;			// Maximum of possible players that can connect to this server
	netadr_t	adr;
} server_cache_t;

#endif