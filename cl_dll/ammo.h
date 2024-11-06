#ifndef __AMMO_H__
#define __AMMO_H__

#define MAX_WEAPON_NAME 128


#define WEAPON_FLAGS_SELECTONEMPTY	1

struct WEAPON
{
	char	szName[MAX_WEAPON_NAME];
	int		iAmmoType;
	int		iAmmo2Type;
	int		iMax1;
	int		iMax2;
	int		iSlot;
	int		iSlotPos;
	int		iFlags;
	int		iId;
	int		iClip;

	int		iCount;		// # of itesm in plist

	HSPRITE_t hActive;
	wrect_t rcActive;
	HSPRITE_t hInactive;
	wrect_t rcInactive;
	HSPRITE_t	hAmmo;
	wrect_t rcAmmo;
	HSPRITE_t hAmmo2;
	wrect_t rcAmmo2;
	HSPRITE_t hCrosshair;
	wrect_t rcCrosshair;
	HSPRITE_t hAutoaim;
	wrect_t rcAutoaim;
};

typedef int AMMO;


#endif