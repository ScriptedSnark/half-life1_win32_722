// cvar.c -- dynamic variable tracking

#include "quakedef.h"
#include "winquake.h"

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

/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
qboolean	Cvar_Command( void )
{
	cvar_t* v;

// check variables
	v = Cvar_FindVar(Cmd_Argv(0));
	if (!v)
		return FALSE;

// perform a variable print or set
	if (Cmd_Argc() == 1)
	{
		Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
		return TRUE;
	}

	Cvar_Set(v->name, Cmd_Argv(1));
	return TRUE;
}


/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables( FILE* f )
{
	cvar_t* var;

	for (var = cvar_vars; var; var = var->next)
		if (var->archive)
			fprintf(f, "%s \"%s\"\n", var->name, var->string);
}


/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the profile flag set to true.
============
*/
void Cvar_WriteProfileVariables( FILE* f )
{
	cvar_t* var;

	for (var = cvar_vars; var; var = var->next)
		if (var->profile)
			fprintf(f, "%s \"%s\"\n", var->name, var->string);
}


/*
============
Cmd_CvarListPrintCvar

Print some info to the console
============
*/
void Cmd_CvarListPrintCvar( cvar_t* var, FILE* f )
{
	char szOutstr[256];   // Ouput string
	if (var->value == (int)var->value)   // Clean up integers
		sprintf(szOutstr, "%-15s : %8i", var->name, (int)var->value);
	else
		sprintf(szOutstr, "%-15s : %8.3f", var->name, var->value);

	// Tack on archive setting
	if (var->archive)
	{
		strcat(szOutstr, ", a");
	}

	// And server setting
	if (var->server)
	{
		strcat(szOutstr, ", sv");
	}

	// and profile setting
	if (var->profile)
	{
		strcat(szOutstr, ", p");
	}

	// End the line
	strcat(szOutstr, "\n");

	Con_Printf("%s", szOutstr);

	if (f)
	{
		fprintf(f, "%s", szOutstr);
	}
}

/*
============
Cmd_CvarList_f

List all cvars
============
*/
void Cmd_CvarList_f( void )
{
	cvar_t* var;			// Temporary Pointer to cvars
	int iCvars = 0;			// Number retrieved...
	int iArgs;				// Argument count
	char* partial = NULL;	// Partial cvar to search for...
	// E.eg
	int ipLen = 0;			// Length of the partial cvar

	char szTemp[256];
	FILE* f = NULL;         // FilePointer for logging
	qboolean bLogging = FALSE;

	iArgs = Cmd_Argc();		// Get count

	if (iArgs >= 2)			// Check for "CvarList ?" or "CvarList xxx"
	{
		if (!_stricmp(Cmd_Argv(1), "?"))
		{
			Con_Printf(
				"CvarList           : List all cvars\n"
				"CvarList [Partial] : List cvars starting with 'Partial'\n"
				"CvarList log logfile [Partial] : Logs cvars to file c:\\logfile.\n"
				"NOTE:  No relative paths allowed!");
			return;
		}

		if (!_stricmp(Cmd_Argv(1), "log"))
		{
			sprintf(szTemp, "c:\\%s", Cmd_Argv(2));
			f = fopen(szTemp, "wt");
			if (f)
				bLogging = TRUE;
			else
			{
				Con_Printf("Couldn't open [%s] for writing!\n", Cmd_Argv(2));
				fclose(f);
			}

			if (iArgs == 4)
			{
				partial = Cmd_Argv(3);
				ipLen = strlen(partial);
			}
		}
		else
		{
			partial = Cmd_Argv(1);
			ipLen = strlen(partial);
		}
	}

	// Banner
	Con_Printf("CVar List\n--------------\n");

	// Loop through cvars...
	for (var = cvar_vars; var; var = var->next)
	{
		if (partial)  // Partial string searching?
		{
			if (!_strnicmp(var->name, partial, ipLen))
			{
				iCvars++;
				Cmd_CvarListPrintCvar(var, f);
			}
		}
		else		  // List all cvars
		{
			iCvars++;
			Cmd_CvarListPrintCvar(var, f);
		}
	}

	// Show total and syntax help...
	if ((iArgs == 2) && partial && partial[0])
	{
		Con_Printf("--------------\n%3i CVars for [%s]\nCvarList ? for syntax\n", iCvars, partial);
	}
	else
	{
		Con_Printf("--------------\n%3i Total CVars\nCvarList ? for syntax\n", iCvars);
	}

	if (bLogging)
	{
		fclose(f);
	}
}

/*
============
Cvar_CountServerVariables

============
*/
int Cvar_CountServerVariables( void )
{
	int i = 0;
	cvar_t* var;

	for (var = cvar_vars; var; var = var->next)
	{
		if (var->server)
		{
			i++;
		}
	}

	return i;
}

void Cvar_CmdInit( void )
{
	Cmd_AddCommand("cvarlist", Cmd_CvarList_f);
}