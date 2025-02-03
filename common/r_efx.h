#if !defined ( R_EFXH )
#define R_EFXH
#ifdef _WIN32
#pragma once
#endif

// TEMPENTITY flags
#define	FTENT_NONE				0x00000000
#define	FTENT_SINEWAVE			0x00000001
#define	FTENT_GRAVITY			0x00000002
#define FTENT_ROTATE			0x00000004
#define	FTENT_SLOWGRAVITY		0x00000008
#define FTENT_SMOKETRAIL		0x00000010
#define FTENT_COLLIDEWORLD		0x00000020
#define FTENT_FLICKER			0x00000040
#define FTENT_FADEOUT			0x00000080
#define FTENT_SPRANIMATE		0x00000100
#define FTENT_HITSOUND			0x00000200
#define FTENT_SPIRAL			0x00000400
#define FTENT_SPRCYCLE			0x00000800
#define FTENT_COLLIDEALL		0x00001000 // will collide with world and slideboxes
#define FTENT_PERSIST			0x00002000 // tent is not removed when unable to draw 
#define FTENT_COLLIDEKILL		0x00004000 // tent is removed upon collision with anything
#define FTENT_PLYRATTACHMENT	0x00008000 // tent is attached to a player (owner)
#define FTENT_SPRANIMATELOOP	0x00010000 // animating sprite doesn't die when last frame is displayed
#define FTENT_SPARKSHOWER		0x00020000
#define FTENT_NOMODEL			0x00040000 // Doesn't have a model, never try to draw ( it just triggers other things )

#endif
