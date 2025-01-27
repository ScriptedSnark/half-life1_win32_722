#ifndef CUSTOMENTITY_H
#define CUSTOMENTITY_H


// Custom Entities
#define CUSTOM_TYPE		( (1<<CUSTOM_SHIFT) | (1<<(CUSTOM_SHIFT+1)) | (1<<(CUSTOM_SHIFT+2)) )
#define CUSTOM_SHIFT	20

#define GET_CUSTOM_TYPE(bits)		((bits & CUSTOM_TYPE)>>CUSTOM_SHIFT)
#define SET_CUSTOM_TYPE(customType)	( ((customType<<CUSTOM_SHIFT)&CUSTOM_TYPE) | U_CUSTOM | U_MOREBITS | U_EVENMOREBITS )
// Space reserved for 8 types of custom entity definitions
#define CUSTOM_BEAM		0
//#define CUSTOM_xxxx		1
//#define CUSTOM_xxxx		2
//#define CUSTOM_xxxx		3
//#define CUSTOM_xxxx		4
//#define CUSTOM_xxxx		5
//#define CUSTOM_xxxx		6
//#define CUSTOM_xxxx		7

//
// NOTE: These cannot collide with U_MOREBITS (1<<0), U_REMOVE (1<<4) U_EVENMOREBITS (1<<8), U_CUSTOM(1<<13) or U_LONGENTITY (1<<15) or any bits above (1<<20)
// SEE: protocol.h
// BEAM encoding bits
//

// skip 1<<0 for U_MOREBITS
#define U_BEAM_STARTX		(1<<1)	// These 3 bits must remain sequential or the code in SV_MAIN.C will have to be changed
#define U_BEAM_STARTY		(1<<2)	// ..
#define U_BEAM_STARTZ		(1<<3)	// ..
// skip 1<<4 for U_REMOVE
#define U_BEAM_ENDX			(1<<5)	// Same with these 3
#define U_BEAM_ENDY			(1<<6)	// ..
#define U_BEAM_ENDZ			(1<<7)	// ..
// skip 1<<8 for U_EVENMOREBITS
#define U_BEAM_ENTS			(1<<9)  // has entity(s)
#define U_BEAM_TYPE			(1<<10)
#define U_BEAM_MODEL		(1<<11)
#define U_BEAM_WIDTH		(1<<12)
// skip 1<<13 for U_CUSTOM
#define U_BEAM_NOISE		(1<<14)
// skip 1<<15 for U_LONGENTITY
#define U_BEAM_RENDER		(1<<16)
#define U_BEAM_BRIGHTNESS	(1<<17)
#define U_BEAM_SCROLL		(1<<18)
#define U_BEAM_FRAME		(1<<19)
// If you add more bits, skip 20-22 (custom types) and add the YETMOREBITS flag to the beam encoder (SV_MAIN.C)
// Skip 1<<23 for U_YETMOREBITS

// Start/End Entity is encoded as 12 bits of entity index, and 4 bits of attachment (4:12)
#define BEAMENT_ENTITY(x)		((x)&0xFFF)
#define BEAMENT_ATTACHMENT(x)	(((x)>>12)&0xF)

// Beam types, encoded as a byte
enum 
{
	BEAM_POINTS = 0,
	BEAM_ENTPOINT,
	BEAM_ENTS,
	BEAM_HOSE,
};

#define BEAM_FSINE		0x10
#define BEAM_FSOLID		0x20
#define BEAM_FSHADEIN	0x40
#define BEAM_FSHADEOUT	0x80

#endif	//CUSTOMENTITY_H
