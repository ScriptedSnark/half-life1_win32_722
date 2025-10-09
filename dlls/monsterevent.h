#ifndef MONSTEREVENT_H
#define MONSTEREVENT_H

typedef struct
{
	int			event;
	char		*options;
} MonsterEvent_t;

#define EVENT_SPECIFIC			0
#define EVENT_SCRIPTED			1000
#define EVENT_SHARED			2000
#define EVENT_CLIENT			5000

#define MONSTER_EVENT_BODYDROP_LIGHT	2001
#define MONSTER_EVENT_BODYDROP_HEAVY	2002

#define MONSTER_EVENT_SWISHSOUND		2010

#endif		// MONSTEREVENT_H
