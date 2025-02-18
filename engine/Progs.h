#ifndef PROGS_H
#define PROGS_H

#include "progdefs.h"

#ifndef EIFACE_H
// Forward declare this type to avoid problems
typedef struct saverestore_s SAVERESTOREDATA;
#endif

// Entity state is used for the baseline and for delta compression of a packet of 
//  entities that is sent to a client.
typedef struct
{
	int     entityType;  // Normal or Custom to know how to parse the entity.
	int     number;      // Index into cl_entities array for this entity.
	int     flags;       // The delta compression bit header.

	vec3_t	origin;
	vec3_t	angles;

	int		modelindex;
	int		sequence;
	float	frame;
	int		colormap;
	short	skin;
	short	solid;
	int		effects;
	float	scale;

	// render information
	int		rendermode;
	int		renderamt;
	color24	rendercolor;
	int		renderfx;

	// Added for entity delta compression
	//vec3_t  msg_origins[2];
	//vec3_t  msg_angles[2];

	int     movetype;
	float   animtime;
	float   framerate;
	int     body;
	byte    controller[4];
	byte    blending[4];
	vec3_t	velocity;

	vec3_t  mins;    // Send bbox down to client for use during prediction.
	vec3_t  maxs;
} entity_state_t;

#define	MAX_ENT_LEAFS	24
typedef struct edict_s
{
	qboolean	free;
	int			serialnumber;
	link_t		area;				// linked to a division node or leaf
	
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	entity_state_t	baseline;
	
	float		freetime;			// sv.time when the object was freed

	void*		pvPrivateData;		// Alloced and freed by engine, used by DLLs

	entvars_t	v;					// C exported fields from progs
// other fields from progs come immediately after
} edict_t;
#define	EDICT_FROM_AREA(l) STRUCT_FROM_LINK(l,edict_t,area)

//============================================================================

extern	char* pr_strings;
extern	globalvars_t	gGlobalVariables;

//============================================================================

edict_t* ED_Alloc( void );
void ED_Free( edict_t* ed );

char* ED_NewString( const char* string );
// returns a copy of the string allocated from the server's string heap

void ED_Print( edict_t* ed );
void ED_Write( SAVERESTOREDATA* save, edict_t* ed );
char* ED_ParseEdict( char* data, edict_t* ent );


//void ED_WriteGlobals( SAVERESTOREDATA* save );
//void ED_ParseGlobals( char* data );

void ED_LoadFromFile( char* data );

//define EDICT_NUM(n) ((edict_t*)(sv.edicts + (n) * pr_edict_size))
//define NUM_FOR_EDICT(e) (((byte*)(e) - sv.edicts) / pr_edict_size)

edict_t* EDICT_NUM( int n );
int NUM_FOR_EDICT( const edict_t* e );

void SuckOutClassname( char* data, edict_t* ent );

void ReleaseEntityDLLFields( edict_t* ent );
void InitEntityDLLFields( edict_t* ent );
void FreeEntPrivateData( edict_t* ent );

#define	EDICT_TO_PROG(e) ((byte*)e - (byte*)sv.edicts)
#define PROG_TO_EDICT(e) ((edict_t*)((byte*)sv.edicts + e))

//============================================================================

#if 0
#define	G_FLOAT(o) (pr_globals[o])
#define	G_INT(o) (*(int *)&pr_globals[o])
#define	G_EDICT(o) ((edict_t *)((byte *)sv.edicts+ *(int *)&pr_globals[o]))
#define G_EDICTNUM(o) NUM_FOR_EDICT(G_EDICT(o))
#define	G_VECTOR(o) (&pr_globals[o])
#define	G_STRING(o) (pr_strings + *(string_t *)&pr_globals[o])
#define	G_FUNCTION(o) (*(func_t *)&pr_globals[o])

#define	E_FLOAT(e,o) (((float*)&e->v)[o])
#define	E_INT(e,o) (*(int *)&((float*)&e->v)[o])
#define	E_VECTOR(e,o) (&((float*)&e->v)[o])
#endif
#define	E_STRING(e,o) (pr_strings + *(string_t *)&((char *)&e->v)[o])

extern	int		type_size[8];

void ED_PrintEdicts( void );
void ED_PrintNum( int ent );


#endif // PROGS_H