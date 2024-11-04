// cvar.c -- dynamic variable tracking

#include "quakedef.h"

cvar_t* cvar_vars;
char* cvar_null_string = "";

/*
============
Cvar_FindVar
============
*/
cvar_t* Cvar_FindVar( char* var_name )
{
	cvar_t* var;

	for (var = cvar_vars; var; var = var->next)
		if (!Q_strcmp(var_name, var->name))
			return var;

	return NULL;
}

/*
============
Cvar_VariableValue
============
*/
float Cvar_VariableValue( char* var_name )
{
	cvar_t* var;

	var = Cvar_FindVar(var_name);
	if (!var)
		return 0;
	return Q_atof(var->string);
}

/*
============
Cvar_VariableInt
============
*/
int Cvar_VariableInt( char* var_name )
{
	cvar_t* var;

	var = Cvar_FindVar(var_name);
	if (!var)
		return 0;
	return Q_atoi(var->string);
}


/*
============
Cvar_VariableString
============
*/
char* Cvar_VariableString( char* var_name )
{
	cvar_t* var;

	var = Cvar_FindVar(var_name);
	if (!var)
		return cvar_null_string;
	return var->string;	
}


/*
============
Cvar_CompleteVariable
============
*/
char* Cvar_CompleteVariable( char* partial )
{
	cvar_t* cvar;
	char* name = NULL;
	int			len;

	len = Q_strlen(partial);

	if (!len)
		return NULL;

// check functions
	for (cvar = cvar_vars; cvar; cvar = cvar->next)
	{
		if (!Q_strncmp(partial, cvar->name, len))
		{
			if (Q_strlen(cvar->name) == len)
				return cvar->name;
			name = cvar->name;
		}
	}

	return name;
}


/*
============
Cvar_Set
============
*/
void Cvar_Set( char* var_name, char* value )
{
	cvar_t* var;
	qboolean changed;

	var = Cvar_FindVar(var_name);
	if (!var)
	{
		Con_DPrintf("Cvar_Set: variable %s not found\n", var_name);
		return;
	}

	changed = Q_strcmp(var->string, value);

	Z_Free(var->string);	// free the old value string

	var->string = Z_Malloc(Q_strlen(value) + 1);
	Q_strcpy(var->string, value);
	var->value = Q_atof(var->string);
	if (var->server && changed)
	{
		if (sv.active)
			SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, var->string);
	}
}

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue( char* var_name, float value )
{
	char	val[32];

	sprintf(val, "%f", value);
	Cvar_Set(var_name, val);
}


/*
============
Cvar_RegisterVariable

Adds a freestanding variable to the variable list.
============
*/
void Cvar_RegisterVariable( cvar_t* variable )
{
	char* oldstr;

// first check to see if it has allready been defined
	if (Cvar_FindVar(variable->name))
	{
		Con_Printf("Can't register variable %s, allready defined\n", variable->name);
		return;
	}

// check for overlap with a command
	if (Cmd_Exists(variable->name))
	{
		Con_Printf("Cvar_RegisterVariable: %s is a command\n", variable->name);
		return;
	}

// copy the value off, because future sets will Z_Free it
	oldstr = variable->string;
	variable->string = Z_Malloc(Q_strlen(oldstr) + 1);
	Q_strcpy(variable->string, oldstr);
	variable->value = Q_atof(variable->string);

// link the variable in
	variable->next = cvar_vars;
	cvar_vars = variable;
}






