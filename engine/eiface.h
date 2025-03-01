#ifndef EIFACE_H
#define EIFACE_H

#ifdef HLDEMO_BUILD
#define INTERFACE_VERSION       001
#else  // !HLDEMO_BUILD, i.e., regular version of HL
#define INTERFACE_VERSION		128
#endif // !HLDEMO_BUILD

#include "custom.h"

#include "cvardef.h"
//
// Defines entity interface between engine and DLLs.
// This header file included by engine files and DLL files.
//
// Before including this header, DLLs must:
//		include progdefs.h
// This is conveniently done for them in extdll.h
//


#define DLLEXPORT __stdcall

typedef enum
{
	at_notice,
	at_console,		// same as at_notice, but forces a ConPrintf, not a message box
	at_aiconsole,	// same as at_console, but only shown if developer level is 2!
	at_warning,
	at_error,
} ALERT_TYPE;

// 4-22-98  JOHN: added for use in pfnClientPrintf
typedef enum
{
	print_console,
	print_center,
	print_chat,
} PRINT_TYPE;


// Returned by TraceLine
typedef struct
{
	int		fAllSolid;			// if true, plane is not valid
	int		fStartSolid;		// if true, the initial point was in a solid area
	int		fInOpen;
	int		fInWater;
	float	flFraction;			// time completed, 1.0 = didn't hit anything
	vec3_t	vecEndPos;			// final position
	float	flPlaneDist;
	vec3_t	vecPlaneNormal;		// surface normal at impact
	edict_t* pHit;				// entity the surface is on
	int		iHitgroup;			// 0 == generic, non zero is specific body part
} TraceResult;

// CD audio status
typedef struct
{
	int	fPlaying;// is sound playing right now?
	int	fWasPlaying;// if not, CD is paused if WasPlaying is true.
	int	fInitialized;
	int	fEnabled;
	int	fPlayLooping;
	float	cdvolume;
	//BYTE 	remap[100];
	int	fCDRom;
	int	fPlayTrack;
} CDStatus;

typedef unsigned long CRC32_t;

// Engine hands this to DLLs for functionality callbacks
typedef struct enginefuncs_s
{
	int			(*pfnPrecacheModel)			( char* s );
	int			(*pfnPrecacheSound)			( char* s );
	void		(*pfnSetModel)				( edict_t* e, char* m );
	int			(*pfnModelIndex)			( char* m );
	int			(*pfnModelFrames)			( int modelIndex );
	void		(*pfnSetSize)				( edict_t* e, float* rgflMin, float* rgflMax );
	void		(*pfnChangeLevel)			( char* s1, char* s2 );
	void		(*pfnGetSpawnParms)			( edict_t* ent );
	void		(*pfnSaveSpawnParms)		( edict_t* ent );
	float		(*pfnVecToYaw)				( float* rgflVector );
	void		(*pfnVecToAngles)			( float* rgflVectorIn, float* rgflVectorOut );
	void		(*pfnMoveToOrigin)			( edict_t* ent, float* pflGoal, float dist, int iMoveType );
	void		(*pfnChangeYaw)				( edict_t* ent );
	void		(*pfnChangePitch)			( edict_t* ent );
	edict_t*	(*pfnFindEntityByString)	( edict_t* pEdictStartSearchAfter, char* pszField, char* pszValue );
	int			(*pfnGetEntityIllum)		( edict_t* pEnt );
	edict_t*	(*pfnFindEntityInSphere)	( edict_t* pEdictStartSearchAfter, float* org, float rad );
	edict_t*	(*pfnFindClientInPVS)		( edict_t* pEdict );
	edict_t*	(*pfnEntitiesInPVS)			( edict_t* pplayer );
	void		(*pfnMakeVectors)			( float* rgflVector );
	void		(*pfnAngleVectors)			( float* rgflVector, float* forward, float* right, float* up );
	edict_t*	(*pfnCreateEntity)			( void );
	void		(*pfnRemoveEntity)			( edict_t* e );
	edict_t*	(*pfnCreateNamedEntity)		( int className );
	void		(*pfnMakeStatic)			( edict_t* ent );
	int			(*pfnEntIsOnFloor)			( edict_t* e );
	int			(*pfnDropToFloor)			( edict_t* e );
	int			(*pfnWalkMove)				( edict_t* ent, float yaw, float dist, int iMode );
	void		(*pfnSetOrigin)				( edict_t* e, float* rgflOrigin );
	void		(*pfnEmitSound)				( edict_t* entity, int channel, char* sample, /*int*/float volume, float attenuation, int fFlags, int pitch );
	void		(*pfnEmitAmbientSound)		( edict_t* entity, float* pos, char* samp, float vol, float attenuation, int fFlags, int pitch );
	void		(*pfnTraceLine)				( float* v1, float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr );
	void		(*pfnTraceToss)				( edict_t* pent, edict_t* pentToIgnore, TraceResult* ptr );
	int			(*pfnTraceMonsterHull)		( edict_t*, float*, float*, int, edict_t*, TraceResult*);
	void		(*pfnTraceHull)				( float* v1, float* v2, int fNoMonsters, int hullNumber, edict_t* pentToSkip, TraceResult* ptr );
	void		(*pfnTraceModel)			( float* v1, float* v2, edict_t* pent, TraceResult* ptr );
	const char*	(*pfnTraceTexture)			( edict_t* pTextureEntity, const float* v1, const float* v2 );
	void		(*pfnTraceSphere)			( float* v1, float* v2, int fNoMonsters, float radius, edict_t* pentToSkip, TraceResult* ptr );
	void		(*pfnGetAimVector)			( edict_t* ent, float speed, float* rgflReturn );
	void		(*pfnServerCommand)			( char* str );
	void		(*pfnClientCommand)			( edict_t* pEdict, char* szFmt, ... );
	void		(*pfnParticleEffect)		( float* org, float* dir, float color, float count );
	void		(*pfnLightStyle)			( int style, char* val );
	int			(*pfnDecalIndex)			( const char* name );
	int			(*pfnPointContents)			( float* rgflVector );
	void		(*pfnMessageBegin)			( int msg_dest, int msg_type, float* pOrigin, edict_t* ed );
	void		(*pfnMessageEnd)			( void );
	void		(*pfnWriteByte)				( int iValue );
	void		(*pfnWriteChar)				( int iValue );
	void		(*pfnWriteShort)			( int iValue );
	void		(*pfnWriteLong)				( int iValue );
	void		(*pfnWriteAngle)			( float flValue );
	void		(*pfnWriteCoord)			( float flValue );
	void		(*pfnWriteString)			( char* sz );
	void		(*pfnWriteEntity)			( int iValue );
	float		(*pfnCVarGetFloat)			( const char* szVarName );
	const char*	(*pfnCVarGetString)			( const char* szVarName );
	void		(*pfnCVarSetFloat)			( const char* szVarName, float flValue );
	void		(*pfnCVarSetString)			( const char* szVarName, const char* szValue );
	void		(*pfnAlertMessage)			( ALERT_TYPE atype, char* szFmt, ... );
	void		(*pfnEngineFprintf)			( FILE* pfile, char* szFmt, ... );
	void*		(*pfnPvAllocEntPrivateData)	( edict_t* pEdict, long cb );
	void*		(*pfnPvEntPrivateData)		( edict_t* pEdict );
	void		(*pfnFreeEntPrivateData)	( edict_t* pEdict );
	char*		(*pfnSzFromIndex)			( int iString );
	int			(*pfnAllocString)			( char* szValue );
	entvars_t*	(*pfnGetVarsOfEnt)			( edict_t* pEdict );
	edict_t*	(*pfnPEntityOfEntOffset)	( int iEntOffset );
	int			(*pfnEntOffsetOfPEntity)	( const edict_t* pEdict );
	int			(*pfnIndexOfEdict)			( const edict_t* pEdict );
	edict_t*	(*pfnPEntityOfEntIndex)		( int iEntIndex );
	edict_t*	(*pfnFindEntityByVars)		( entvars_t* pvars );
	void*		(*pfnGetModelPtr)			( edict_t* pEdict );
	int			(*pfnRegUserMsg)			( const char* pszName, int iSize);
	void		(*pfnAnimationAutomove)		( const edict_t* pEdict, float flTime );
	void		(*pfnGetBonePosition)		( const edict_t* pEdict, int iBone, float* rgflOrigin, float* rgflAngles );
	uint32		(*pfnFunctionFromName)		( char* pName );
	char*		(*pfnNameForFunction)		( uint32 function );
	void		(*pfnClientPrintf)			( edict_t* pEdict, PRINT_TYPE ptype, char* szMsg ); // JOHN: engine callbacks so game DLL can print messages to individual clients
	char*		(*pfnCmd_Args)				( void );		// these 3 added 
	char*		(*pfnCmd_Argv)				( int argc );	// so game DLL can easily 
	int			(*pfnCmd_Argc)				( void );		// access client 'cmd' strings
	void		(*pfnGetAttachment)			( const edict_t* pEdict, int iAttachment, float* rgflOrigin, float* rgflAngles );
	void		(*pfnCRC32_Init)			( CRC32_t* pulCRC );
	void		(*pfnCRC32_ProcessBuffer)	( CRC32_t* pulCRC, void* p, int len );
	void		(*pfnCRC32_ProcessByte)		( CRC32_t* pulCRC, unsigned char ch );
	int			(*pfnCRC32_Final)			( CRC32_t pulCRC );
	int			(*pfnRandomLong)			( long  lLow, long  lHigh );
	float		(*pfnRandomFloat)			( float flLow, float flHigh );
	void		(*pfnSetView)				( const edict_t* pClient, const edict_t* pViewent );
	float		(*pfnTime)					( void );
	void		(*pfnCrosshairAngle)		( const edict_t* pClient, float pitch, float yaw );
	byte*		(*pfnLoadFileForMe)			( char* filename, int* pLength );
	void		(*pfnFreeFile)				( void* buffer );
	void		(*pfnEndSection)			( const char* pszSectionName ); // trigger_endsection
	int			(*pfnCompareFileTime)		( char* filename1, char* filename2, int* iCompare );
	void		(*pfnGetGameDir)			( char* szGetGameDir );
	void		(*pfnCvar_RegisterVariable)	( cvar_t* variable );
	void		(*pfnFadeClientVolume)		( const edict_t* pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds );
	void		(*pfnSetClientMaxspeed)		( const edict_t* pEdict, float fNewMaxspeed );
	edict_t*	(*pfnCreateFakeClient)		( const char* netname );	// returns NULL if fake client can't be created
	void		(*pfnRunPlayerMove)			( edict_t* fakeclient, float* viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec );
	int			(*pfnNumberOfEntities)		( void );
	int			(*pfnIsMapValid)			( char* filename );
} enginefuncs_t;
// be sure to bump INTERFACE_VERSION if you add a new function

// Passed to pfnKeyValue
typedef struct KeyValueData_s
{
	char* szClassName;	// in: entity classname
	char* szKeyName;		// in: name of key
	char* szValue;		// in: value of key
	long	fHandled;		// out: DLL sets to true if key-value pair was understood
} KeyValueData;


typedef struct
{
	char		mapName[32];
	char		landmarkName[32];
	edict_t*	pentLandmark;
	vec3_t		vecLandmarkOrigin;
} LEVELLIST;
#define MAX_LEVEL_CONNECTIONS	16		// These are encoded in the lower 16bits of ENTITYTABLE->flags

typedef struct
{
	int			id;				// Ordinal ID of this entity (used for entity <--> pointer conversions)
	edict_t*	pent;			// Pointer to the in-game entity

	int			location;		// Offset from the base data of this entity
	int			size;			// Byte size of this entity's data
	int			flags;			// This could be a short -- bit mask of transitions that this entity is in the PVS of
	string_t	classname;		// entity class name

} ENTITYTABLE;

#define FENTTABLE_PLAYER		0x80000000
#define FENTTABLE_REMOVED		0x40000000
#define FENTTABLE_MOVEABLE		0x20000000
#define FENTTABLE_GLOBAL		0x10000000

typedef struct saverestore_s
{
	char*		pBaseData;		// Start of all entity save data
	char*		pCurrentData;	// Current buffer pointer for sequential access
	int			size;			// Current data size
	int			bufferSize;		// Total space for data
	int			tokenSize;		// Size of the linear list of tokens
	int			tokenCount;		// Number of elements in the pTokens table
	char**		pTokens;		// Hash table of entity strings (sparse)
	int			currentIndex;	// Holds a global entity table ID
	int			tableCount;		// Number of elements in the entity table
	int			connectionCount;// Number of elements in the levelList[]
	ENTITYTABLE* pTable;		// Array of ENTITYTABLE elements (1 for each entity)
	LEVELLIST	levelList[MAX_LEVEL_CONNECTIONS];		// List of connections from this level

	// smooth transition
	int			fUseLandmark;
	char		szLandmarkName[20];// landmark we'll spawn near in next level
	vec3_t		vecLandmarkOffset;// for landmark transitions
	float		time;
	char		szCurrentMapName[32];	// To check global entities

} SAVERESTOREDATA;

typedef enum _fieldtypes
{
	FIELD_FLOAT = 0,		// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_ENTITY,			// An entity offset (EOFFSET)
	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EVARS,			// EVARS *
	FIELD_EDICT,			// edict_t *, or edict_t *  (same thing)
	FIELD_VECTOR,			// Any vector
	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_POINTER,			// Arbitrary data pointer... to be removed, use an array of FIELD_CHARACTER
	FIELD_INTEGER,			// Any integer or enum
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

	FIELD_TYPECOUNT,		// MUST BE LAST
} FIELDTYPE;

#ifndef offsetof
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

#define _FIELD(type,name,fieldtype,count,flags)		{ fieldtype, #name, offsetof(type, name), count, flags }
#define DEFINE_FIELD(type,name,fieldtype)			_FIELD(type, name, fieldtype, 1, 0)
#define DEFINE_ARRAY(type,name,fieldtype,count)		_FIELD(type, name, fieldtype, count, 0)
#define DEFINE_ENTITY_FIELD(name,fieldtype)			_FIELD(entvars_t, name, fieldtype, 1, 0 )
#define DEFINE_ENTITY_GLOBAL_FIELD(name,fieldtype)	_FIELD(entvars_t, name, fieldtype, 1, FTYPEDESC_GLOBAL )
#define DEFINE_GLOBAL_FIELD(type,name,fieldtype)	_FIELD(type, name, fieldtype, 1, FTYPEDESC_GLOBAL )


#define FTYPEDESC_GLOBAL			0x0001		// This field is masked for global entity save/restore

typedef struct
{
	FIELDTYPE		fieldType;
	char* fieldName;
	int				fieldOffset;
	short			fieldSize;
	short			flags;
} TYPEDESCRIPTION;

#define Q_ARRAYSIZE(p)		(sizeof(p)/sizeof(p[0]))

typedef struct
{
	int				(*pfnSpawn)				( edict_t* pent );
	void			(*pfnThink)				( edict_t* pent );
	void			(*pfnUse)				( edict_t* pentUsed, edict_t* pentOther );
	void			(*pfnTouch)				( edict_t* pentTouched, edict_t* pentOther );
	void			(*pfnBlocked)			( edict_t* pentBlocked, edict_t* pentOther );
	void			(*pfnKeyValue)			( edict_t* pentKeyvalue, KeyValueData* pkvd );
	void			(*pfnSave)				( edict_t* pent, SAVERESTOREDATA* pSaveData );
	int				(*pfnRestore)			( edict_t* pent, SAVERESTOREDATA* pSaveData, int globalEntity );
	void			(*pfnSetAbsBox)			( edict_t* pent );

	void			(*pfnSaveWriteFields)	( SAVERESTOREDATA* , char* , void* , TYPEDESCRIPTION* , int );
	void			(*pfnSaveReadFields)	( SAVERESTOREDATA* , char* , void* , TYPEDESCRIPTION* , int );

	void			(*pfnSaveGlobalState)	( SAVERESTOREDATA* );
	void			(*pfnRestoreGlobalState)	(SAVERESTOREDATA*);
	void			(*pfnResetGlobalState)		( void );

	void			(*pfnClientConnect)		( edict_t* pEntity );
	void			(*pfnClientDisconnect)	( edict_t* pEntity );
	void			(*pfnClientKill)		( edict_t* pEntity );
	void			(*pfnClientPutInServer)	( edict_t* pEntity );
	void			(*pfnClientCommand)		( edict_t* pEntity ); // JOHN: client 'cmd' commands are passed through to the DLL

	void			(*pfnServerActivate)	( edict_t* pEdictList, int edictCount, int clientMax );

	void			(*pfnPlayerPreThink)	( edict_t* pEntity );
	void			(*pfnPlayerPostThink)	( edict_t* pEntity );

	void			(*pfnStartFrame)		( void );
	void			(*pfnParmsNewLevel)		( void );
	void			(*pfnParmsChangeLevel)	( void );

	char*			(*pfnGetGameDescription)( void );      // Returns string describing current .dll.  E.g., TeamFotrress 2, Half-Life
														   // More descriptive than just com_gamedir.

	void			(*pfnPlayerCustomization) ( edict_t* pEntity, customization_t* pCustom );  // Notify dll about a player customization.

	// Spectator funcs
	void			(*pfnSpectatorConnect)		( edict_t* pEntity );
	void			(*pfnSpectatorDisconnect)	( edict_t* pEntity );
	void			(*pfnSpectatorThink)		( edict_t* pEntity );
} DLL_FUNCTIONS;

extern DLL_FUNCTIONS	gEntityInterface;

typedef int	(*APIFUNCTION)( DLL_FUNCTIONS* pFunctionTable, int interfaceVersion );


#endif EIFACE_H