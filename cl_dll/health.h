#define DMG_IMAGE_LIFE		2	// seconds that image is up

#define DMG_IMAGE_POISON	0
#define DMG_IMAGE_ACID		1
#define DMG_IMAGE_COLD		2
#define DMG_IMAGE_DROWN		3
#define DMG_IMAGE_BURN		4
#define DMG_IMAGE_NERVE		5
#define DMG_IMAGE_RAD		6
#define DMG_IMAGE_SHOCK		7
#define NUM_DMG_TYPES		8

// instant damage
#define DMG_GENERIC			0			// generic damage was done
#define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FREEZE			(1 << 4)	// frozen
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_CLUB			(1 << 7)	// crowbar, punch, headbutt
#define DMG_SHOCK			(1 << 8)	// electric shock
#define DMG_SONIC			(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam 
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.


// time-based damage
#define DMG_DROWN			(1 << 14)	// Drowning
#define DMG_FIRSTTIMEBASED  DMG_DROWN

#define DMG_PARALYZE		(1 << 15)	// slows affected creature down
#define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
#define DMG_POISON			(1 << 17)	// blood poisioning
#define DMG_RADIATION		(1 << 18)	// radiation exposure
#define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
#define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
#define DMG_SLOWBURN		(1 << 21)	// in an oven
#define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#define DMG_MORTAR			(1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)


typedef struct
{
	float fExpire;
	float fBaseline;
	int	x, y;
} DAMAGE_IMAGE;
	
//
//-----------------------------------------------------
//
class CHudHealth: public CHudBase
{
public:
	virtual int Init( void );
	virtual int VidInit( void );
	virtual int Draw(float fTime);
	virtual void Reset( void );
	int MsgFunc_Health(char *pszName,  int iSize, void *pbuf);
	int MsgFunc_Damage(char *pszName,  int iSize, void *pbuf);
	int m_iHealth;

private:
	HSPRITE_t m_hSprite;
	HSPRITE_t m_hDamage;
	
	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	int	m_bitsDamage;
	float m_fFade;
	void GetPainColor( int &r, int &g, int &b );
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	void CalcDamageDirection(vec3_t vecFrom);
	void UpdateTiles(float fTime, long bits);
};	
