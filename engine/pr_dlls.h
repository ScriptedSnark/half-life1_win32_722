// pr_dlls.h
#ifndef PR_DLLS_H
#define PR_DLLS_H
#pragma once

// Name of game listing file
#define GAME_LIST_FILE       "liblist.gam"

typedef struct
{
	uint32	pFunction;
	char*	pFunctionName;
} functiontable_t;

typedef struct extensiondll_s
{
	void*	lDLLHandle;
	functiontable_t* functionTable;
	int		functionCount;
} extensiondll_t;

#define MAX_EXT_DLLS		50

extern int g_iextdllMac;
extern extensiondll_t g_rgextdll[MAX_EXT_DLLS];

typedef void (*ENTITYINIT)			( struct entvars_s* );
typedef void (*DISPATCHFUNCTION)	( struct entvars_s* , void* );
typedef void (*FIELDIOFUNCTION)		( SAVERESTOREDATA* , const char* , void* , TYPEDESCRIPTION* , int );


#endif // PR_DLLS_H