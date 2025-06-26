//
//  cdll_dll.h

// this file is included by both the game-dll and the client-dll,

#ifndef CDLL_DLL_H
#define CDLL_DLL_H

#define MAX_WEAPONS		32		// ???

#define MAX_WEAPON_SLOTS		5	// hud item selection slots
#define MAX_ITEM_TYPES			11	// hud item selection slots

#define MAX_ITEMS				14	// hard coded item types

#define	HIDEHUD_WEAPONS		( 1<<0 )
#define	HIDEHUD_FLASHLIGHT	( 1<<1 )
#define	HIDEHUD_ALL			( 1<<2 )
#define 	HIDEHUD_HEALTH		( 1<<3 )

#define	MAX_AMMO_TYPES	20		// ???
#define MAX_AMMO_SLOTS  20		// not really slots

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4


#define WEAPON_SUIT			31

#endif