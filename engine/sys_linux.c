// sys_linux.c -- Linux system interface code

#define _GNU_SOURCE

#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"
#include "pr_edict.h"
#include "r_studio.h"
#include "host_cmd.h"
#include "exefuncs.h"
#include "gameinfo.h"
#include "profile.h"
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>

SDL_Window* pmainwindow;

uint32_t				gProcessorSpeed;
int					gHasMMXTechnology;

qboolean			Win32AtLeastV4;

qboolean			g_bInStartup = FALSE;
qboolean			g_bInactive = FALSE;

qboolean			gfUseLANAuthentication = TRUE;

char				g_szProfileName[MAX_QPATH];
qboolean			g_bForceReloadOnCA_Active = FALSE;

// 0 = not active, 1 = active, 2 = pause
int					giActive = DLL_INACTIVE;
int					giStateInfo;

static double pfreq;
static double curtime = 0.0;
static double lastcurtime = 0.0;
static int lowshift;
static struct timespec start_time;

qboolean			isDedicated;

int					g_bPaused = 0;
int					gLauncherPause = 0;

void	(*VID_LockBuffer)( void );
void	(*VID_UnlockBuffer)( void );
void	(*VID_Shutdown)( void );
void	(*VID_Update)( struct vrect_s* rects );
int		(*VID_ForceUnlockedAndReturnState)( void );
void	(*VID_SetDefaultMode)( void );
char*	(*VID_GetExtModeDescription)( int mode );
int		(*VID_ForceLockState)( int lk );
void	(*VID_GetVID)( struct viddef_s* pvid );
int		(*D_SurfaceCacheForRes)( int width, int height );
void	(*D_BeginDirectRect)( int width, int height, byte* data, int pic_width, int pic_height );
void	(*D_EndDirectRect)( int width, int height, int pic_width, int pic_height );
void	(*AppActivate)( int fActive, int minimize );
void	(*CDAudio_Play)( int track, int looping );
void	(*CDAudio_Pause)( void );
void	(*CDAudio_Resume)( void );
void	(*CDAudio_Update)( void );
void	(*ErrorMessage)( int nLevel, const char* pszErrorMessage );
void	(*Console_Printf)( char* fmt, ... );
void	(*Launcher_InitCmds)( void );
void	(*Launcher_GetCDKey)( char* pszCDKey, int* nLength, int* bDedicated );
int		(*Launcher_GetClientID)( struct clientid_s* pID );
void	(*Launcher_GetUUID)( void* pUUID, int* nLength, int* bDedicated );
char*	(*Launcher_VerifyMessage)( int nLength, byte* pKey, int nMsgLength, char* pMsg, int nSignLength, byte* pSign );
int		(*Launcher_GetCertificate)( void* pBuffer, int* nLength );
int		(*Launcher_RequestNewClientCertificate)( void );
int		(*Launcher_ValidateClientCertificate)( void* pBuffer, int nLength );

void Sys_InitFloatTime( void );
void SeedRandomNumberGenerator( void );

void MaskExceptions( void );
void Sys_PushFPCW_SetHigh( void );
void Sys_PopFPCW( void );
void Sys_TruncateFPU( void );

/*
===============================================================================

PROFILE SYSTEM

===============================================================================
*/

/*
================
Sys_GetProfileRegKeyValue

Gets profile settings from INI file
================
*/
void Sys_GetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszReturnString, int nReturnLength, char* pszDefaultValue )
{
	FILE* file;
	char line[256];
	char key[256];
	char value[256];
	char fileName[512];
	char section[256];
	qboolean found = FALSE;
	qboolean inCorrectSection = FALSE;
	
	strncpy(pszReturnString, pszDefaultValue, nReturnLength);
	pszReturnString[nReturnLength - 1] = NULL;
	
	if (pszSetting && pszSetting[0])
		snprintf(fileName, sizeof(fileName), "%s_%s.ini", pszName, pszSetting);
	else
		snprintf(fileName, sizeof(fileName), "%s.ini", pszName);
	
	char* lastSlash = strrchr(pszPath, '\\');
	if (lastSlash)
		strncpy(section, lastSlash + 1, sizeof(section));
	else
		strncpy(section, pszPath, sizeof(section));
	
	file = fopen(fileName, "r");
	if (!file)
	{
		Sys_SetProfileRegKeyValue(pszName, pszPath, pszSetting, pszElement, pszDefaultValue);
		return;
	}
	
	while (fgets(line, sizeof(line), file))
	{
		line[strcspn(line, "\r\n")] = NULL;
		
		if (line[0] == NULL)
			continue;
			
		if (line[0] == '[' && line[strlen(line)-1] == ']')
		{
			char currentSection[256];
			strncpy(currentSection, line + 1, sizeof(currentSection) - 1);
			currentSection[strlen(currentSection) - 1] = NULL;
			
			inCorrectSection = (strcmp(currentSection, section) == NULL);
			continue;
		}
		
		if (inCorrectSection)
		{
			char* separator = strchr(line, '=');
			if (separator)
			{
				*separator = NULL;
				strncpy(key, line, sizeof(key));
				strncpy(value, separator + 1, sizeof(value));
				
				char* end = key + strlen(key) - 1;
				while (end > key && isspace((unsigned char)*end)) end--;
				end[1] = NULL;
				
				char* start = key;
				while (*start && isspace((unsigned char)*start))
					start++;

				if (start != key)
					memmove(key, start, strlen(start) + 1);
				
				if (!strcmp(key, pszElement))
				{
					strncpy(pszReturnString, value, nReturnLength);
					pszReturnString[nReturnLength - 1] = NULL;
					found = TRUE;
					break;
				}
			}
		}
	}
	
	fclose(file);
	
	if (!found)
		Sys_SetProfileRegKeyValue(pszName, pszPath, pszSetting, pszElement, pszDefaultValue);
}

/*
================
Sys_SetProfileRegKeyValue

Sets a profile value in INI file
================
*/
void Sys_SetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszValue )
{
	FILE* file;
	FILE* tempFile;
	char line[256];
	char key[256];
	char fileName[512];
	char tempFileName[512];
	char section[256];
	qboolean found = FALSE;
	qboolean inCorrectSection = FALSE;
	qboolean sectionExists = FALSE;
	
	if (pszSetting && pszSetting[0])
		snprintf(fileName, sizeof(fileName), "%s_%s.ini", pszName, pszSetting);
	else
		snprintf(fileName, sizeof(fileName), "%s.ini", pszName);
	
	char* lastSlash = strrchr(pszPath, '\\');
	if (lastSlash)
		strncpy(section, lastSlash + 1, sizeof(section));
	else
		strncpy(section, pszPath, sizeof(section));
	
	snprintf(tempFileName, sizeof(tempFileName), "%s.tmp", fileName);
	
	file = fopen(fileName, "r");
	tempFile = fopen(tempFileName, "w");
	
	if (!tempFile)
	{
		if (file)
			fclose(file);

		tempFile = fopen(fileName, "w");
		if (!tempFile)
			return;
		
		fprintf(tempFile, "[%s]\n", section);
		fprintf(tempFile, "%s=%s\n", pszElement, pszValue);
		fclose(tempFile);

		return;
	}
	
	if (file)
	{
		while (fgets(line, sizeof(line), file))
		{
			char* newline = strchr(line, '\n');
			if (newline)
				*newline = NULL;
			
			if (line[0] == '[' && line[strlen(line)-1] == ']')
			{
				char currentSection[256];
				strncpy(currentSection, line + 1, sizeof(currentSection) - 1);
				currentSection[strlen(currentSection) - 1] = NULL;
				
				if (inCorrectSection && !found)
				{
					fprintf(tempFile, "%s=%s\n", pszElement, pszValue);
					found = 1;
				}
				
				inCorrectSection = (!strcmp(currentSection, section));
				sectionExists = (sectionExists || inCorrectSection);
				
				fprintf(tempFile, "[%s]\n", currentSection);
				continue;
			}
			
			if (inCorrectSection && !found)
			{
				char* separator = strchr(line, '=');
				if (separator)
				{
					*separator = NULL;
					strncpy(key, line, sizeof(key));
					
					char* end = key + strlen(key) - 1;
					while (end > key && isspace((unsigned char)*end))
						end--;

					end[1] = NULL;
					
					char* start = key;
					while (*start && isspace((unsigned char)*start))
						start++;

					if (start != key)
						memmove(key, start, strlen(start) + 1);
					
					if (!strcmp(key, pszElement))
					{

						fprintf(tempFile, "%s=%s\n", pszElement, pszValue);
						found = TRUE;
						continue;
					}
					else
					{
						*separator = '=';
					}
				}
			}
			
			fprintf(tempFile, "%s\n", line);
		}
		fclose(file);
	}
	
	if (!found)
	{
		if (!sectionExists)
			fprintf(tempFile, "[%s]\n", section);
		else if (!inCorrectSection)
			fprintf(tempFile, "[%s]\n", section);

		fprintf(tempFile, "%s=%s\n", pszElement, pszValue);
	}
	
	fclose(tempFile);
	remove(fileName);
	rename(tempFileName, fileName);
}

static char keynames[256][32];

/*
================
Profile_ClearKeyBindings

Release all profile key bindings
================
*/
void Profile_ClearKeyBindings( player_profile_t* pProfile )
{
	int i;
	player_keybinding_t* kb;

	for (i = 0; i < 256; i++)
	{
		kb = &pProfile->keybindings[i];
		if (kb->binding)
		{
			free(kb->binding);
			kb->binding = NULL;
		}
		memset(kb, 0, sizeof(player_keybinding_t));
	}
}

/*
================
Profile_GetKeyIndexByName

Release all profile key bindings
================
*/
int Profile_GetKeyIndexByName( char* pszKeyName, char* pszKeyArray )
{
	int i;

	if (!pszKeyName || !pszKeyName[0])
		return -1;

	for (i = 0; i < 256; i++)
	{
		if (pszKeyArray && pszKeyArray[0])
		{
			if (!_stricmp(pszKeyArray, pszKeyName))
				return i;
		}

		pszKeyArray += 32;
	}
	return -1;
}

/*
================
Profile_LoadKeyNames

Load key names from kb_keys.lst
================
*/
void Profile_LoadKeyNames( void )
{
	int	i;
	char szPath[MAX_PATH];
	char* pParsePos;
	byte* pFileData;

	memset(keynames, 0, sizeof(keynames));

	strcpy(szPath, "gfx/shell/kb_keys.lst");
	pFileData = COM_LoadFile(szPath, 5, NULL);
	if (!pFileData)
	{
		Con_DPrintf("Could not load kb_keys.lst\n");
		return;
	}

	pParsePos = pFileData;
	for (i = 0; i < 256; i++)
	{
		pParsePos = COM_Parse(pParsePos);
		if (!com_token[0])
			break;
		if (!_strnicmp(com_token, "<UNK", 4))
			continue;

		strcpy(keynames[i], com_token);
	}
	
	free(pFileData);
}

/*
================
Profile_LoadDefaultKeyBindings

Load default key bindings from kb_def.lst
================
*/
void Profile_LoadDefaultKeyBindings( player_profile_t* pProfile )
{
	char szPath[MAX_OSPATH];
	char szKey[32];
	char szBinding[256];
	int index, size;
	char* pParsePos;
	byte* pFileData;
	player_keybinding_t* kb;

	Profile_LoadKeyNames();

	strcpy(szPath, "gfx/shell/kb_def.lst");
	pFileData = COM_LoadFile(szPath, 5, NULL);
	if (!pFileData)
		return;

	pParsePos = pFileData;
	while (1)
	{
		// Parse key name
		pParsePos = COM_Parse(pParsePos);
		if (!com_token[0])
			break;
		strcpy(szKey, com_token);

		// Parse binding
		pParsePos = COM_Parse(pParsePos);
		if (!strlen(com_token))
			break;
		strcpy(szBinding, com_token);

		index = Profile_GetKeyIndexByName(szKey, keynames[0]);
		if (index == -1)
			continue;

		kb = &pProfile->keybindings[index];
		size = strlen(szBinding) + 1;
		kb->binding = malloc(size);
		if (!kb->binding)
			break;

		memset(kb->binding, 0, size);
		strcpy(kb->binding, szBinding);
		strcpy(pProfile->name, szKey);
		pProfile->size = size - 1;
	}

	free(pFileData);
}

/*
================
Profile_InitDefaultSettings

Initialize profile with default settings
================
*/
qboolean Profile_InitDefaultSettings( player_profile_t* pProfile )
{
	Profile_LoadKeyNames();

	pProfile->keybindings[255].lookstrafe = 0.0;
	pProfile->keybindings[255].lookspring = 0.0;
	pProfile->keybindings[255].windowed_mouse = 1.0;
	pProfile->keybindings[255].crosshair = 0.0;
	pProfile->keybindings[255].mfilter = 0.0;
	pProfile->keybindings[255].mlook = 1.0;
	pProfile->keybindings[255].joystick = 0.0;
	pProfile->keybindings[255].m_pitch = 0.022;
	pProfile->keybindings[255].sensitivity = 3.0;

	pProfile->brightness = 1.0;
	pProfile->bgmvolume = 1.0;
	pProfile->viewsize = 120.0;
	pProfile->hisound = 1.0;
	pProfile->a3d = 1.0;
	pProfile->gamma = 2.5;
	pProfile->suitvolume = 0.25;
	pProfile->volume = 0.8;

	return TRUE;
}

/*
================
Profile_Load

Load player profile
================
*/
qboolean Profile_Load( char* pszName, player_profile_t* pProfile )
{
	if (!pszName || !pProfile)
		return FALSE;

	Profile_ClearKeyBindings(pProfile);

	memset(pProfile, 0, sizeof(player_profile_t));

	if (!Profile_InitDefaultSettings(pProfile))
		return FALSE;

	Profile_LoadKeyBindings(pszName, pProfile);
	Profile_LoadSettings(pszName, pProfile);
	return TRUE;
}

/*
================
Profile_Save

Save player profile
================
*/
int Profile_Save( char* pszName, player_profile_t* pProfile )
{
	if (!pszName || !pProfile)
		return FALSE;

	Profile_SaveKeyBindings(pszName, pProfile);
	Profile_SaveSettings(pszName, pProfile);
	return TRUE;
}

/*
================
Profile_LoadKeyBindings

Load profile key bindings
================
*/
void Profile_LoadKeyBindings( char* pszName, player_profile_t* pProfile )
{
	int	i;
	int	size;
	char szPath[256];
	char szElement[256];
	char szValue[256];
	player_keybinding_t* kb;

	if (!pProfile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");
	COM_FixSlashes(szPath);

	for (i = 0; i < 256; i++)
	{
		kb = &pProfile->keybindings[i];

		sprintf(szElement, "%03i", i);

		if (kb->binding && kb->binding[0])
			strcpy(szValue, kb->binding);
		else
			sprintf(szValue, "");

		Sys_GetProfileRegKeyValue(pszName, szPath, "KB", szElement, szValue, sizeof(szValue), szValue);

		if (szValue[0])
		{
			if (kb->binding)
				free(kb->binding);

			size = strlen(szValue) + 1;
			kb->binding = (char*)malloc(size);
			if (!kb->binding)
				Sys_Error("Could not allocate space for key binding");

			memset(kb->binding, 0, size);
			strcpy(kb->binding, szValue);

			pProfile->size = size;
		}
		else
		{
			if (kb->binding)
				free(kb->binding);

			kb->binding = NULL;

			pProfile->size = 0;
		}
	}
}

/*
================
Profile_SaveKeyBindings

Save profile key bindings
================
*/
void Profile_SaveKeyBindings( char* pszName, player_profile_t* pProfile )
{
	int	i;
	char szPath[256];
	char szElement[256];
	char szValue[256];
	player_keybinding_t* kb;

	if (!pProfile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");

	COM_FixSlashes(szPath);
	for (i = 0; i < 256; i++)
	{
		kb = &pProfile->keybindings[i];

		sprintf(szElement, "%03i", i);

		if (kb->binding && kb->binding[0])
			strcpy(szValue, kb->binding);
		else
			sprintf(szValue, "");

		Sys_SetProfileRegKeyValue(pszName, szPath, "KB", szElement, szValue);
	}
}

/*
================
Profile_LoadSettings

Load profile settings
================
*/
void Profile_LoadSettings( char* pszName, player_profile_t* pProfile )
{
	char szPath[256];
	char szValue[256];

	if (!pProfile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");
	COM_FixSlashes(szPath);

	sprintf(szValue, "%f", pProfile->keybindings[255].lookstrafe);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "lookstrafe", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].lookstrafe = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].lookspring);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "lookspring", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].lookspring = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].crosshair);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "crosshair", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].crosshair = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].windowed_mouse);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "windowed_mouse", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].windowed_mouse = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].m_pitch);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "m_pitch", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].m_pitch = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].mfilter);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "mfilter", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].mfilter = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].mlook);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "mlook", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].mlook = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].joystick);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "joystick", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].joystick = atof(szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].sensitivity);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "sensitivity", szValue, sizeof(szPath), szValue);
	pProfile->keybindings[255].sensitivity = atof(szValue);

	sprintf(szValue, "%f", pProfile->viewsize);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "viewsize", szValue, sizeof(szPath), szValue);
	pProfile->viewsize = atof(szValue);

	sprintf(szValue, "%f", pProfile->brightness);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "brightness", szValue, sizeof(szPath), szValue);
	pProfile->brightness = atof(szValue);

	sprintf(szValue, "%f", pProfile->gamma);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "gamma", szValue, sizeof(szPath), szValue);
	pProfile->gamma = atof(szValue);

	sprintf(szValue, "%f", pProfile->bgmvolume);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "bgmvolume", szValue, sizeof(szPath), szValue);
	pProfile->bgmvolume = atof(szValue);

	sprintf(szValue, "%f", pProfile->suitvolume);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "suitvolume", szValue, sizeof(szPath), szValue);
	pProfile->suitvolume = atof(szValue);

	sprintf(szValue, "%f", pProfile->hisound);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "hisound", szValue, sizeof(szPath), szValue);
	pProfile->hisound = atof(szValue);

	sprintf(szValue, "%f", pProfile->volume);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "volume", szValue, sizeof(szPath), szValue);
	pProfile->volume = atof(szValue);

	sprintf(szValue, "%f", pProfile->a3d);
	Sys_GetProfileRegKeyValue(pszName, szPath, "CVAR", "a3d", szValue, sizeof(szPath), szValue);
	pProfile->a3d = atof(szValue);
	Cvar_Set("a3d", szValue);
}

/*
================
Profile_SaveSettings

Save profile settings
================
*/
void Profile_SaveSettings( char* pszName, player_profile_t* pProfile )
{
	char szPath[256];
	char szValue[256];

	if (!pProfile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");
	COM_FixSlashes(szPath);

	sprintf(szValue, "%f", pProfile->keybindings[255].lookstrafe);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "lookstrafe", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].lookspring);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "lookspring", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].crosshair);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "crosshair", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].windowed_mouse);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "windowed_mouse", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].m_pitch);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "m_pitch", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].mfilter);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "mfilter", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].mlook);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "mlook", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].joystick);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "joystick", szValue);

	sprintf(szValue, "%f", pProfile->keybindings[255].sensitivity);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "sensitivity", szValue);

	sprintf(szValue, "%f", pProfile->viewsize);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "viewsize", szValue);

	sprintf(szValue, "%f", pProfile->brightness);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "brightness", szValue);

	sprintf(szValue, "%f", pProfile->gamma);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "gamma", szValue);

	sprintf(szValue, "%f", pProfile->bgmvolume);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "bgmvolume", szValue);

	sprintf(szValue, "%f", pProfile->suitvolume);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "suitvolume", szValue);

	sprintf(szValue, "%f", pProfile->hisound);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "hisound", szValue);

	sprintf(szValue, "%f", pProfile->volume);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "volume", szValue);

	sprintf(szValue, "%f", pProfile->a3d);
	Sys_SetProfileRegKeyValue(pszName, szPath, "CVAR", "a3d", szValue);
}

volatile int					sys_checksum;


/*
================
Sys_PageIn
================
*/
void Sys_PageIn( void* ptr, int size )
{
	byte* x;
	int		m, n;

// touch all the memory to make sure it's there. The 16-page skip is to
// keep Win 95 from thinking we're trying to page ourselves in (we are
// doing that, of course, but there's no reason we shouldn't)
	x = (byte*)ptr;

	for (n = 0; n < 4; n++)
	{
		for (m = 0; m < (size - 16 * 0x1000); m += 4)
		{
			sys_checksum += *(int*)&x[m];
			sys_checksum += *(int*)&x[m + 16 * 0x1000];
		}
	}
}


/*
===============================================================================

FILE IO

===============================================================================
*/

#define	MAX_HANDLES		10
FILE* sys_handles[MAX_HANDLES];

int	findhandle( void )
{
	int		i;

	for (i = 1; i < MAX_HANDLES; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength( FILE* f )
{
	int		pos;
	int		end;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	VID_ForceLockState(t);

	return end;
}

int Sys_FileOpenRead( char* path, int* hndl )
{
	FILE* f;
	int		i, retval;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	i = findhandle();

	f = fopen(path, "rb");

	if (!f)
	{
		*hndl = -1;
		retval = -1;
	}
	else
	{
		sys_handles[i] = f;
		*hndl = i;
		retval = filelength(f);
	}

	VID_ForceLockState(t);

	return retval;
}

int Sys_FileOpenWrite( char* path )
{
	FILE* f;
	int		i;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	i = findhandle();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	sys_handles[i] = f;

	VID_ForceLockState(t);

	return i;
}

void Sys_FileClose( int handle )
{
	int		t;

	t = VID_ForceUnlockedAndReturnState();
	fclose(sys_handles[handle]);
	sys_handles[handle] = NULL;
	VID_ForceLockState(t);
}

void Sys_FileSeek( int handle, int position )
{
	int		t;

	t = VID_ForceUnlockedAndReturnState();
	fseek(sys_handles[handle], position, SEEK_SET);
	VID_ForceLockState(t);
}

int Sys_FileRead( int handle, void* dest, int count )
{
	int		t, x;

	t = VID_ForceUnlockedAndReturnState();
	x = fread(dest, 1, count, sys_handles[handle]);
	VID_ForceLockState(t);
	return x;
}

int Sys_FileWrite( int handle, void* data, int count )
{
	int		t, x;
	
	t = VID_ForceUnlockedAndReturnState();
	x = fwrite(data, 1, count, sys_handles[handle]);
	VID_ForceLockState(t);
	return x;
}

int	Sys_FileTime( char* path )
{
	FILE* f;
	int		t, retval;

	t = VID_ForceUnlockedAndReturnState();

	f = fopen(path, "rb");

	if (f)
	{
		fclose(f);
		retval = 1;
	}
	else
	{
		retval = -1;
	}

	VID_ForceLockState(t);
	return retval;
}

void Sys_mkdir(char* path)
{
	mkdir(path, 0755);
}

int Sys_FileTell( int i )
{
	return ftell(sys_handles[i]);
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable( unsigned long startaddr, unsigned long length )
{
	
}


#ifndef _M_IX86

void Sys_SetFPCW( void )
{
}

void Sys_PushFPCW_SetHigh( void )
{
}

void Sys_PopFPCW( void )
{
}

void Sys_TruncateFPU( void )
{
}

void MaskExceptions( void )
{
}

#endif

/*
================
Sys_Init
================
*/
void Sys_Init( void )
{
	MaskExceptions();
	Sys_SetFPCW();

	// Use clock_gettime for Linux high-resolution timer
	if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0)
		Sys_Error("No hardware timer available");

	// pfreq is 1.0 / nanoseconds per second
	pfreq = 1.0 / 1000000000.0;

	Sys_InitFloatTime();
}


void Sys_Error( char* error, ... )
{
	va_list		argptr;
	char		text[1024];
	static qboolean bReentry = FALSE; // Don't meltdown

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	if (bReentry)
	{
		fprintf(stderr, "%s\n", text);
		longjmp(host_abortserver, 2);
	}

	bReentry = TRUE;

	VID_ForceUnlockedAndReturnState();

	if (isDedicated)
	{
		vsprintf(text, error, argptr);
		if (Console_Printf)
		{
			Console_Printf("Error %s\n", text);
		}
	}
	else
	{
		if (ErrorMessage)
		{
			ErrorMessage(0, text);
		}
	}
}

void Sys_Warning( char* fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	VID_ForceUnlockedAndReturnState();

	if (isDedicated)
	{
		vsprintf(text, fmt, argptr);
		if (Console_Printf)
		{
			Console_Printf("WARNING:  %s\n", text);
		}
	}
	else
	{
		if (ErrorMessage)
		{
			ErrorMessage(1, text);
		}
	}

	giActive = DLL_PAUSED;
}

void Sys_Printf( char* fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	if (isDedicated)
	{
		va_start(argptr, fmt);
		vsprintf(text, fmt, argptr);
		va_end(argptr);

		if (Console_Printf)
		{
			Console_Printf(text);
		}
	}
}

void Sys_Quit( void )
{
	VID_ForceUnlockedAndReturnState();

	Host_Shutdown();

	giActive = DLL_CLOSE;

	longjmp(host_abortserver, 1);
}


/*
================
Sys_FloatTime
================
*/
DLL_EXPORT double Sys_FloatTime( void )
{
	static int sametimecount;
	static double oldtime;
	static int first = 1;
	struct timespec now;
	double time, temp;

	Sys_PushFPCW_SetHigh();

	clock_gettime(CLOCK_MONOTONIC, &now);

	temp = (double)(now.tv_sec - start_time.tv_sec) +
	       (double)(now.tv_nsec - start_time.tv_nsec) * pfreq;

	if (first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
		if ((temp <= oldtime) && ((oldtime - temp) < 1.0))
		{
			oldtime = temp;
		}
		else
		{
			time = temp - oldtime;
			oldtime = temp;
			curtime += time;

			if (curtime == lastcurtime)
			{
				sametimecount++;
				if (sametimecount > 100000)
				{
					curtime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}
			lastcurtime = curtime;
		}
	}

	Sys_PopFPCW();

	return curtime;
}


/*
================
Sys_InitFloatTime
================
*/
void Sys_InitFloatTime( void )
{
	int		j;

	Sys_FloatTime();

	j = COM_CheckParm("-starttime");

	if (j)
	{
		curtime = (double)(Q_atof(com_argv[j + 1]));
	}
	else
	{
		curtime = 0.0;
	}

	lastcurtime = curtime;
}

void Sys_Sleep( void )
{
	usleep(1000); // Sleep for 1 millisecond
}


void Sys_SendKeyEvents(void)
{
	SDL_Event event;

	if (g_bInStartup)
		return;

	// Process all pending events
	while (SDL_PollEvent(&event))
	{
		// Any event means we should update screen
		scr_skipupdate = 0;

		// Check for quit event
		if (event.type == SDL_QUIT)
			Sys_Quit();

		// Handle activation (focus) events
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					g_bInactive = false;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					g_bInactive = true;
					break;
			}
		}
	}

	// If window is inactive, clear out queued focus events only (optional)
	if (g_bInactive)
	{
		while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0)
		{
			scr_skipupdate = 0;
			//App_HandleEvent(&event);
		}
	}

	scr_skipupdate = 0;
}


/*
==============================================================================

 WINDOWS CRAP

==============================================================================
*/


/*
==================
WinMain
==================
*/

int	giSubState;

void Dispatch_Substate( int iSubState )
{
	giSubState = iSubState;
}

DLL_EXPORT void GameSetSubState( int iSubState )
{
	if (iSubState & 2)
	{
		Dispatch_Substate(1);
	}
	else if (iSubState != 1)
	{
		Dispatch_Substate(iSubState);
	}
}

DLL_EXPORT void GameSetState( int iState )
{
	giActive = iState;
}

qboolean gfBackground;

DLL_EXPORT void GameSetBackground( qboolean bNewSetting )
{
	gfBackground = bNewSetting;
}


// Called when the game starts up
char* argv[MAX_NUM_ARGVS];
DLL_EXPORT int GameInit(const char* lpCmdLine, unsigned char* pMem, int iSize, exefuncs_t* pef, void* pwnd, const char* profile, int bIsDedicated, char* szBaseDir )
{
	quakeparms_t parms;

	host_initialized = FALSE;

	memset(g_szProfileName, 0, sizeof(g_szProfileName));
	strncpy(g_szProfileName, profile, sizeof(g_szProfileName) - 1);

	// SDL window
	pmainwindow = (void*)pwnd;

	// Initialize exe funcs
	Console_Printf                         = pef->Console_Printf;

	VID_LockBuffer                         = pef->VID_LockBuffer;
	VID_UnlockBuffer                       = pef->VID_UnlockBuffer;
	VID_Shutdown                           = pef->VID_Shutdown;
	VID_Update                             = pef->VID_Update;
	VID_ForceLockState                     = pef->VID_ForceLockState;
	VID_ForceUnlockedAndReturnState        = pef->VID_ForceUnlockedAndReturnState;
	VID_SetDefaultMode                     = pef->VID_SetDefaultMode;
	VID_GetExtModeDescription              = pef->VID_GetExtModeDescription;

	D_SurfaceCacheForRes                   = pef->D_SurfaceCacheForRes;

	VID_GetVID                             = pef->VID_GetVID;

	D_BeginDirectRect                      = pef->D_BeginDirectRect;
	D_EndDirectRect                        = pef->D_EndDirectRect;

	AppActivate                            = pef->AppActivate;

	CDAudio_Play                           = pef->CDAudio_Play;
	CDAudio_Resume                         = pef->CDAudio_Resume;
	CDAudio_Pause                          = pef->CDAudio_Pause;
	CDAudio_Update                         = pef->CDAudio_Update;

	gHasMMXTechnology                      = pef->fMMX;
	gProcessorSpeed                        = pef->iCPUMhz;

	ErrorMessage                           = pef->ErrorMessage;

	Launcher_InitCmds                      = pef->Launcher_InitCmds;
	Launcher_GetCDKey                      = pef->Launcher_GetCDKey;
	Launcher_GetClientID                   = pef->Launcher_GetClientID;
	Launcher_VerifyMessage                 = pef->Launcher_VerifyMessage;

	Launcher_GetCertificate                = pef->Launcher_GetCertificate;
	Launcher_RequestNewClientCertificate   = pef->Launcher_RequestNewClientCertificate;
	Launcher_ValidateClientCertificate     = pef->Launcher_ValidateClientCertificate;

	// Remove Win32 version check
	Win32AtLeastV4 = TRUE;

	SeedRandomNumberGenerator();

	// You may skip or implement Sys_TruncateFPU() if needed
	// Sys_TruncateFPU();

	parms.basedir = szBaseDir;
	parms.cachedir = NULL;

	// Manual command-line parsing (only if lpCmdLine is still used)
	parms.argc = 1;
	argv[0] = "";

	const char* p = lpCmdLine;
	while (*p && (parms.argc < MAX_NUM_ARGVS))
	{
		while (*p && ((*p <= 32) || (*p > 126))) p++;

		if (*p)
		{
			argv[parms.argc++] = (char*)p;

			while (*p && (*p > 32) && (*p <= 126)) p++;

			if (*p)
			{
				*(char*)p = '\0';
				p++;
			}
		}
	}

	parms.argv = argv;
	COM_InitArgv(parms.argc, parms.argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	isDedicated = bIsDedicated;

	parms.membase = pMem;
	parms.memsize = iSize;

	Sys_Init();

	S_BlockSound();

	Sys_Printf("Host_Init\n");
	if (!Host_Init(&parms))
		return FALSE;

	LoadEntityDLLs(host_parms.basedir);

	NET_Config(TRUE);

	return TRUE;
}

DLL_EXPORT int GetGameInfo( struct GameInfo_s* pGI, char* pszChannel )
{
	GameInfo_t gi;
	memset(&gi, 0, sizeof(GameInfo_t));

	gi.build_number = build_number();
	gi.multiplayer = svs.maxclients != 1;
	gi.maxclients = svs.maxclients;

	strncpy(gi.levelname, sv.name, sizeof(gi.levelname) - 1);

	gi.active = sv.active;

	if (net_local_adr.type == NA_IP)
	{
		*(uint32*)gi.ip = *(uint32*)net_local_adr.ip;
		gi.port = net_local_adr.port;
	}

	if (pszChannel && pszChannel[0] && SV_CheckChannel(pszChannel))
		gi.inroom = TRUE;

	gi.state = cls.state;
	gi.signon = cls.signon;

	memset(gi.szStatus, 0, sizeof(gi.szStatus));
	switch (cls.state)
	{
	case ca_active:
		sprintf(gi.szStatus, "Currently in a game");
		break;
	case ca_disconnected:
		sprintf(gi.szStatus, "Disconnected");
		break;
	case ca_uninitialized:
		if (cls.download)
		{
			sprintf(gi.szStatus, "Downloading %s, %i percent complete", cls.downloadname, cls.downloadpercent);
		}
		else
		{
			sprintf(gi.szStatus, "Initializing and downloading");
		}
		break;
	case ca_connected:
		sprintf(gi.szStatus, "Connected to server");
		break;
	case ca_connecting:
		sprintf(gi.szStatus, "Connecting");
		break;
	case ca_dedicated:
		sprintf(gi.szStatus, "Running dedicated server");
		break;
	default:
		sprintf(gi.szStatus, "?");
		break;
	}

	memcpy(pGI, &gi, sizeof(GameInfo_t));
	return TRUE;
}

// Reload user profile and apply it's settings
void ExecuteProfileSettings( char* pszName )
{
	int i;
	char* keyname, * binding;

	player_profile_t profile;
	memset(&profile, 0, sizeof(profile));

	Profile_Load(pszName, &profile);

	// Change the name
	if (Q_strcmp(cl_name.string, pszName))
	{
		Cvar_Set("_cl_name", pszName);
		Host_ClearSaveDirectory();
	}

	// Set key bindings
	for (i = 0; i < 256; i++)
	{
		keyname = Key_KeynumToString(i);
		if (!keyname)
			continue;
		if (!_strnicmp(keyname, "<UNK", 4))
			continue;

		binding = profile.keybindings[i].binding;
		if (binding && binding[0])
		{
			Key_SetBinding(i, binding);

			if (strlen(keyname) == 1)
			{
				if (keyname[0] >= 'A' && keyname[0] <= 'Z')
					Key_SetBinding(keyname[0] + K_SPACE, binding);
			}
		}
	}

	Cvar_SetValue("lookstrafe", profile.keybindings[255].lookstrafe);
	Cvar_SetValue("lookspring", profile.keybindings[255].lookspring);
	Cvar_SetValue("crosshair", profile.keybindings[255].crosshair);
	Cvar_SetValue("_windowed_mouse", profile.keybindings[255].windowed_mouse);
	Cvar_SetValue("m_pitch", profile.keybindings[255].m_pitch);
	Cvar_SetValue("m_filter", profile.keybindings[255].mfilter);
	Cvar_SetValue("joystick", profile.keybindings[255].joystick);

	if (profile.keybindings[255].mlook)
	{
		Cbuf_AddText("+mlook\n");
		Cvar_SetValue("lookstrafe", 0.0);
		Cvar_SetValue("lookspring", 0.0);
	}
	else
	{
		Cbuf_AddText("-mlook\n");
	}

	Cvar_SetValue("sensitivity", profile.keybindings[255].sensitivity);
	Cvar_SetValue("viewsize", profile.viewsize);
	Cvar_SetValue("brightness", profile.brightness);
	Cvar_SetValue("gamma", profile.gamma);
	Cvar_SetValue("bgmvolume", profile.bgmvolume);
	Cvar_SetValue("suitvolume", profile.suitvolume);
	Cvar_SetValue("hisound", profile.hisound);
	Cvar_SetValue("volume", profile.volume);
	Cvar_SetValue("a3d", profile.a3d);

	Profile_ClearKeyBindings(&profile);

	Cbuf_AddText("exec autoexec.cfg\n");
	Cbuf_Execute();
}

DLL_EXPORT void ForceReloadProfile( char* pszName )
{
	memset(g_szProfileName, 0, sizeof(g_szProfileName));
	strcpy(g_szProfileName, pszName);

	g_bForceReloadOnCA_Active = TRUE;
	ExecuteProfileSettings(pszName);
}

DLL_EXPORT void SetStartupMode( qboolean bMode )
{
	g_bInStartup = bMode;
}

DLL_EXPORT void SetMessagePumpDisableMode( qboolean bMode )
{
	g_bInactive = bMode;
}

DLL_EXPORT void SetPauseState( qboolean bPause )
{
	if (bPause)
	{
		if (!gLauncherPause)
			g_bPaused = sv.paused;

		sv.paused = TRUE;
		gLauncherPause = TRUE;
	}
	else
	{
		if (gLauncherPause)
			sv.paused = g_bPaused;

		gLauncherPause = FALSE;
	}
}

DLL_EXPORT int GetPauseState( void )
{
	return sv.paused;
}

DLL_EXPORT void Keyboard_ReturnToGame( void )
{
	Con_DPrintf("KB Reset\n");
}

// The table of exported engine functions to game dll
static enginefuncs_t g_engfuncsExportedToDlls =
{
	PF_precache_model_I,
	PF_precache_sound_I,
	PF_setmodel_I,
	PF_modelindex,
	ModelFrames,
	PF_setsize_I,
	PF_changelevel_I,
	PF_setspawnparms_I,
	SaveSpawnParms,
	PF_vectoyaw_I,
	PF_vectoangles_I,
	SV_MoveToOrigin_I,
	PF_changeyaw_I,
	PF_changepitch_I,
	FindEntityByString,
	GetEntityIllum,
	FindEntityInSphere,
	PF_checkclient_I,
	PVSFindEntities,
	PF_makevectors_I,
	AngleVectors,
	PF_Spawn_I,
	PF_Remove_I,
	CreateNamedEntity,
	PF_makestatic_I,
	PF_checkbottom_I,
	PF_droptofloor_I,
	PF_walkmove_I,
	PF_setorigin_I,
	PF_sound_I,
	PF_ambientsound_I,
	PF_traceline_DLL,
	PF_TraceToss_DLL,
	TraceMonsterHull,
	TraceHull,
	TraceModel,
	TraceTexture,
	TraceSphere,
	PF_aim_I,
	PF_localcmd_I,
	PF_stuffcmd_I,
	PF_particle_I,
	PF_lightstyle_I,
	PF_DecalIndex,
	PF_pointcontents_I,
	PF_MessageBegin_I,
	PF_MessageEnd_I,
	PF_WriteByte_I,
	PF_WriteChar_I,
	PF_WriteShort_I,
	PF_WriteLong_I,
	PF_WriteAngle_I,
	PF_WriteCoord_I,
	PF_WriteString_I,
	PF_WriteEntity_I,
	CVarGetFloat,
	CVarGetString,
	CVarSetFloat,
	CVarSetString,
	AlertMessage,
	EngineFprintf,
	PvAllocEntPrivateData,
	PvEntPrivateData,
	FreeEntPrivateData,
	SzFromIndex,
	AllocEngineString,
	GetVarsOfEnt,
	PEntityOfEntOffset,
	EntOffsetOfPEntity,
	IndexOfEdict,
	PEntityOfEntIndex,
	FindEntityByVars,
	GetModelPtr,
	RegUserMsg,
	AnimationAutomove,
	GetBonePosition,
	FunctionFromName,
	NameForFunction,
	ClientPrintf,
	Cmd_Args,
	Cmd_Argv,
	Cmd_Argc,
	GetAttachment,
	CRC32_Init,
	CRC32_ProcessBuffer,
	CRC32_ProcessByte,
	CRC32_Final,
	RandomLong,
	RandomFloat,
	PF_setview_I,
	PF_Time,
	PF_crosshairangle_I,
	COM_LoadFileForMe,
	COM_FreeFile,
	Host_EndSection,
	COM_CompareFileTime,
	COM_GetGameDir,
	Cvar_RegisterVariable,
	PF_FadeVolume,
	PF_SetClientMaxspeed,
	PF_CreateFakeClient_I,
	PF_RunPlayerMove_I,
	PF_NumberOfEntities_I,
	PF_IsMapValid_I
};

extensiondll_t		g_rgextdll[MAX_EXT_DLLS];
int					g_iextdllMac;

DLL_FUNCTIONS		gEntityInterface;

// Gets a dllexported function from the first DLL that exports it
// Returns functions to dispatch events to entities
DISPATCHFUNCTION GetDispatch( char* pname )
{
	int i;
	DISPATCHFUNCTION pDispatch;

	for (i = 0; i < g_iextdllMac; i++)
	{
		pDispatch = (DISPATCHFUNCTION)Sys_GetProcAddress((HMODULE)g_rgextdll[i].lDLLHandle, pname);
		if (pDispatch)
		{
			return pDispatch;
		}
	}

	return NULL;
}

char* FindAddressInTable( extensiondll_t* pDll, uint32 function )
{
#ifdef _WIN32
	int	i;

	for (i = 0; i < pDll->functionCount; i++)
	{
		if (pDll->functionTable[i].pFunction == function)
			return pDll->functionTable[i].pFunctionName;
	}

	return NULL;
#else
	Dl_info addrInfo;
	if (dladdr(function, &addrInfo))
		return addrInfo.dli_sname;

	return NULL;
#endif
}

uint32 FindNameInTable( extensiondll_t* pDll, char* pName )
{
#ifdef _WIN32
	int	i;

	for (i = 0; i < pDll->functionCount; i++)
	{
		if (!strcmp(pName, pDll->functionTable[i].pFunctionName))
			return pDll->functionTable[i].pFunction;
	}

	return 0;
#else
	return Sys_GetProcAddress(pDll->lDLLHandle, pName);
#endif
}

const char* ConvertNameToLocalPlatform(const char* pchInName)
{
	static char s_szNewName[512];
	char temp[512];
	char *end, *params, *paramEnd, *name;
	int paramBytes, nameBytes;

#ifdef _WIN32
	if (strstr(pchInName, "@"))
	{
		return pchInName;
	}

	if (pchInName[0] != '_' || pchInName[1] != 'Z')
	{
		return "unknown";
	}

	strncpy(temp, pchInName + 3, sizeof(temp));
	temp[sizeof(temp) - 1] = '\0';

	end = &temp[Q_strlen(temp)];
	paramBytes = Q_atoi(temp);

	params = temp;
	while (params < end && IsDigitCharacter(*params))
	{
		++params;
	}

	paramEnd = params + paramBytes;
	nameBytes = Q_atoi(paramEnd);
	*paramEnd = '\0';

	name = paramEnd;

	while (name < end && IsDigitCharacter(*name))
	{
		name++;
	}

	snprintf(s_szNewName, sizeof(s_szNewName), "%s@%s", name, params);
	return s_szNewName;
#elif defined(PLATFORM_LINUX)
	static char s_szNewName_14648[512];
	char szTempName[512];
	char* pchAt;
	char* pchClassName;
	char* pchFunctionName;

	// TODO(ox): Not sure if this works!

	const char* result = pchInName;

	// If not a mangled name, try to convert from @-style to Itanium ABI mangling
	if (pchInName[0] != '_' || pchInName[1] != 'Z')
	{
		pchAt = (char*)strchr(pchInName, '@');
		if (pchAt)
		{
			strncpy(szTempName, pchInName, sizeof(szTempName) - 1);
			szTempName[sizeof(szTempName) - 1] = '\0';

			pchAt = strchr(szTempName, '@');
			if (!pchAt)
				return "unknown";
			pchFunctionName = pchAt + 1;
			*pchAt = '\0';
			pchClassName = szTempName;

			int funcNameLen = (int)strlen(pchFunctionName);
			int classNameLen = (int)strlen(pchClassName);

			// Try _ZN<funcNameLen><funcName><classNameLen><className>Ev
			snprintf(s_szNewName_14648, sizeof(s_szNewName_14648), "_ZN%d%s%d%sEv", funcNameLen, pchFunctionName,
					 classNameLen, pchClassName);
			if (FindNameInTable(&g_rgextdll[0], s_szNewName_14648))
				return s_szNewName_14648;

			// Try _ZN<funcNameLen><funcName><classNameLen><className>EP11CBaseEntity
			snprintf(s_szNewName_14648, sizeof(s_szNewName_14648), "_ZN%d%s%d%sEP11CBaseEntity", funcNameLen,
					 pchFunctionName, classNameLen, pchClassName);
			if (FindNameInTable(&g_rgextdll[0], s_szNewName_14648))
				return s_szNewName_14648;

			// Try _ZN<funcNameLen><funcName><classNameLen><className>EP11CBaseEntityS1_8USE_TYPEf
			snprintf(s_szNewName_14648, sizeof(s_szNewName_14648), "_ZN%d%s%d%sEP11CBaseEntityS1_8USE_TYPEf",
					 funcNameLen, pchFunctionName, classNameLen, pchClassName);
			if (FindNameInTable(&g_rgextdll[0], s_szNewName_14648))
				return s_szNewName_14648;

			return "unknown";
		}
		else
		{
			return "unknown";
		}
	}
	return result;
#else
# error "Not implemented"
#endif
}

// Gets the index of an exported function
uint32 FunctionFromName( char* pName )
{
	int	i;
	uint32 function;

	pName = ConvertNameToLocalPlatform(pName);

	for (i = 0; i < g_iextdllMac; i++)
	{
		function = FindNameInTable(&g_rgextdll[i], pName);
		if (function)
		{
			return function;
		}
	}

	Con_Printf("Can't find proc: %s\n", pName);
	return 0;
}

// Gets the name of an exported function
char* NameForFunction( uint32 function )
{
	int i;
	char* pName;

	for (i = 0; i < g_iextdllMac; i++)
	{
		pName = FindAddressInTable(&g_rgextdll[i], function);
		if (pName)
		{
			return pName;
		}
	}

	Con_Printf("Can't find address: %08lx\n", function);
	return NULL;
}

// Gets a dllexported function from the first DLL that exports it.
// Returns entity initialization functions, generated by LINK_ENTITY_TO_CLASS
ENTITYINIT GetEntityInit( char* pClassName )
{
	return (ENTITYINIT)GetDispatch(pClassName);
}

// Gets a dllexported function from the first DLL that exports it
FIELDIOFUNCTION GetIOFunction( char* pName )
{
	return (FIELDIOFUNCTION)GetDispatch(pName);
}

//
// Scan DLL directory, load all DLLs that conform to spec.
//
void LoadEntityDLLs(char* szBaseDir)
{
	char szDllPath[1024];
	char szGameDir[64];
	FILE* f = NULL;
	char* pszInputStream = NULL;
	int nFileSize = 0;

	g_iextdllMac = 0;
	memset(g_rgextdll, 0, sizeof(g_rgextdll));

	if (COM_CheckParm("-game") || COM_CheckParm("-usegamelist"))
	{
		int i = COM_CheckParm("-game");
		strcpy(szGameDir, (i && i < com_argc - 1) ? com_argv[i + 1] : "valve");

		snprintf(szDllPath, sizeof(szDllPath), "%s/%s/%s", szBaseDir, szGameDir, GAME_LIST_FILE);

		f = fopen(szDllPath, "rb");
		if (!f)
			Sys_Error("Could not load game listing file [%s]", szDllPath);

		fseek(f, 0, SEEK_END);
		nFileSize = ftell(f);
		rewind(f);

		if (nFileSize == 0 || nFileSize > 1024 * 256)
			Sys_Error("Game listing file size is bogus [%s: size %i]", GAME_LIST_FILE, nFileSize);

		pszInputStream = (char*)malloc(nFileSize + 1);
		if (!pszInputStream)
			Sys_Error("Allocation failure for game listing");

		fread(pszInputStream, 1, nFileSize, f);
		pszInputStream[nFileSize] = '\0';
		fclose(f);

		char* pStreamPos = pszInputStream;
		while (1)
		{
			com_ignorecolons = 1;
			pStreamPos = COM_Parse(pStreamPos);
			com_ignorecolons = 0;
			if (!pStreamPos)
				break;

			if (!strstr(com_token, ".so"))
			{
				Con_Printf("Skipping non-so: %s\n", com_token);
				continue;
			}

			snprintf(szDllPath, sizeof(szDllPath), "%s/%s/%s", szBaseDir, szGameDir, com_token);
			Con_Printf("Adding: %s\n", szDllPath);
			LoadThisDll(szDllPath);
		}

		free(pszInputStream);
	}
	else
	{
		// fallback to loading .so from valve/dlls
		snprintf(szDllPath, sizeof(szDllPath), "%s/valve/dlls", szBaseDir);

		DIR* dir = opendir(szDllPath);
		if (!dir)
			Sys_Error("Failed to open directory: %s", szDllPath);

		struct dirent* ent;
		while ((ent = readdir(dir)) != NULL)
		{
			if (strstr(ent->d_name, ".so"))
			{
				char fullPath[1024];
				snprintf(fullPath, sizeof(fullPath), "%s/valve/dlls/%s", szBaseDir, ent->d_name);
				LoadThisDll(fullPath);
			}
		}
		closedir(dir);
	}

	// Load GetEntityAPI
	APIFUNCTION pfnGetAPI = (APIFUNCTION)GetDispatch("GetEntityAPI");
	if (!pfnGetAPI)
		Sys_Error("Can't get DLL API!");

	int interface_version = INTERFACE_VERSION;
	if (!pfnGetAPI(&gEntityInterface, interface_version))
		Sys_Error("Invalid DLL version!");

	Con_Printf("----------------------\n");
	Con_Printf("Dlls loaded for game:\n%s\n", gEntityInterface.pfnGetGameDescription());
	Con_Printf("----------------------\n");
}

//
// Try to load a single DLL.  If it conforms to spec, keep it loaded, and add relevant
// info to the DLL directory.  If not, ignore it entirely.
//
void LoadThisDll( char* szDllFilename )
{
	HMODULE hDLL;
	extensiondll_t* pextdll;
	typedef void (DLLEXPORT* PFN_GiveFnptrsToDll)(enginefuncs_t*, globalvars_t*);
	PFN_GiveFnptrsToDll pfnGiveFnptrsToDll;

	hDLL = Sys_LoadLibrary(szDllFilename);

	if (hDLL == NULL)
	{
		Con_Printf("LoadLibrary failed on %s\n", szDllFilename);
		return;
	}

	pfnGiveFnptrsToDll = (PFN_GiveFnptrsToDll)Sys_GetProcAddress(hDLL, "GiveFnptrsToDll");

	if (!pfnGiveFnptrsToDll)
	{
		Con_Printf("Couldn't get GiveFnptrsToDll in %s\n", szDllFilename);
		Sys_FreeLibrary(hDLL);
		return;
	}

	pfnGiveFnptrsToDll(&g_engfuncsExportedToDlls, &gGlobalVariables);

	if (g_iextdllMac == MAX_EXT_DLLS)
	{
		Con_Printf("Too many DLLs, ignoring remainder\n");
		Sys_FreeLibrary(hDLL);
		return;
	}

	pextdll = &g_rgextdll[g_iextdllMac++];

	memset(pextdll, 0, sizeof(*pextdll));
	pextdll->lDLLHandle = hDLL;

#ifdef _WIN32
	BuildExportTable(pextdll, szDllFilename);
#endif
}


//
// Release all entity dlls
//
void ReleaseEntityDlls( void )
{
	extensiondll_t* pextdll = g_rgextdll;
	extensiondll_t* pextdllMac = &g_rgextdll[g_iextdllMac];

	while (pextdll < pextdllMac)
	{
		Sys_FreeLibrary(pextdll->lDLLHandle);
		pextdll->lDLLHandle = NULL;

		pextdll++;
	}
}

void EngineFprintf( void* pFile, char* szFmt, ... )
{
	va_list		argptr;
	static char szOut[1024];

	va_start(argptr, szFmt);
	vsprintf(szOut, szFmt, argptr);
	va_end(argptr);

	fprintf(pFile, szOut);
}

// Outputs a message to the server console
// If aType is at_logged and this is a multiplayer game, logs the message to the log file
void AlertMessage( ALERT_TYPE atype, char* szFmt, ... )
{
	va_list		argptr;
	static char szOut[1024];

	if (!developer.value)
		return;

	va_start(argptr, szFmt);
	vsprintf(szOut, szFmt, argptr);
	va_end(argptr);

	switch (atype)
	{
	case at_notice:
		Con_Printf("NOTE:  %s", szOut);
		break;
	case at_console:
		Con_Printf("%s", szOut);
		break;
	case at_aiconsole:
		if (developer.value < 2)
			return;
		Con_Printf("%s", szOut);
		break;
	case at_warning:
		Con_Printf("WARNING:  %s", szOut);
		break;
	case at_error:
		Con_Printf("ERROR:  %s", szOut);
		break;
	}
}

DLL_EXPORT int GetEngineState( void )
{
	return cls.state;
}