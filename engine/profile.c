// profile.c - user profiles

#include "quakedef.h"
#include "profile.h"

// Gets profile settings in in the registry
void GetProfileRegKeyValue( char* pszName, char* pszPath, char* pszSetting, char* pszElement,
	char* pszReturnString, int nReturnLength, char* pszDefaultValue )
{
	LONG lResult;           // Registry function result code
	HKEY hKey;              // Handle of opened/created key
	char szBuff[128];		// Temp. buffer
	DWORD dwDisposition;    // Type of key opening event
	DWORD dwType;           // Type of key
	DWORD dwSize;           // Size of element data
	char szSubKey[256];

	sprintf(pszReturnString, pszDefaultValue);

	if (pszSetting && pszSetting[0])
		sprintf(szSubKey, "%s\\%s\\%s", pszPath, pszName, pszSetting);
	else
		sprintf(szSubKey, "%s\\%s", pszPath, pszName);

	lResult = RegCreateKeyEx(
		HKEY_CURRENT_USER,	// handle of open key 
		szSubKey,			// address of name of subkey to open 
		0,					// DWORD ulOptions,	  // reserved 
		"String",			// Type of value
		REG_OPTION_NON_VOLATILE, // Store permanently in reg.
		KEY_ALL_ACCESS,		// REGSAM samDesired, // security access mask 
		NULL,
		&hKey,				// Key we are creating
		&dwDisposition);    // Type of creation

	if (lResult != ERROR_SUCCESS)  // Failure
		return;

	// First time
	if (dwDisposition == REG_CREATED_NEW_KEY)
	{
		// Just Set the Values according to the defaults
		lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (const BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
	}
	else
	{
		// We opened the existing key. Now go ahead and find out how big the key is.
		dwSize = nReturnLength;
		lResult = RegQueryValueEx(hKey, pszElement, 0, &dwType, (unsigned char*)szBuff, &dwSize);

		// Success?
		if (lResult == ERROR_SUCCESS)
		{
			// Only copy strings, and only copy as much data as requested.
			if (dwType == REG_SZ)
			{
				strncpy(pszReturnString, szBuff, nReturnLength);
				pszReturnString[nReturnLength - 1] = 0;
			}
		}
		else
		// Didn't find it, so write out new value
		{
			// Just Set the Values according to the defaults
			lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (const BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
		}
	}

	RegCloseKey(hKey);
}

// TODO: Implement

// Releases all profile key bindings
void ClearProfileKeyBindings( profile_t* profile )
{
	int i;
	profile_keybinding_t* kb;

	for (i = 0; i < 256; i++)
	{
		kb = &profile->keybindings[i];

		if (kb->binding)
		{
			free(kb->binding);
			kb->binding = NULL;
		}

		memset(kb, 0, sizeof(profile_keybinding_t));
	}
}

// TODO: Implement

void StoreProfile( void )
{
	// TODO: Implement
}

// TODO: Implement

int InitProfile( profile_t* profile )
{
	StoreProfile();

	profile->keybindings[255].lookstrafe = 0.0;
	profile->keybindings[255].lookspring = 0.0;
	profile->keybindings[255].windowed_mouse = 1.0;
	profile->keybindings[255].crosshair = 0.0;
	profile->keybindings[255].mfilter = 0.0;
	profile->keybindings[255].mlook = 1.0;
	profile->keybindings[255].joystick = 0.0;
	profile->keybindings[255].m_pitch = 0.022;

	profile->brightness = 1.0;
	profile->bgmvolume = 1.0;
	profile->keybindings[255].sensitivity = 3.0;
	profile->viewsize = 120.0;
	profile->hisound = 1.0;
	profile->a3d = 1.0;
	profile->gamma = 2.5;
	profile->suitvolume = 0.25;
	profile->volume = 0.8;

	return TRUE;
}

int LoadProfile( char* pszName, profile_t* profile )
{
	if (!pszName || !profile)
		return FALSE;

	ClearProfileKeyBindings(profile);

	memset(profile, 0, sizeof(profile_t));

	if (!InitProfile(profile))
		return FALSE;

	LoadProfileKeyBindings(pszName, profile);
	LoadProfileCvars(pszName, profile);

	return TRUE;
}

// TODO: Implement

// Get profile key bindings from registry
void LoadProfileKeyBindings( char* pszName, profile_t* profile )
{
	int	i, size;
	char szPath[256], szElement[256], szValue[256];
	profile_keybinding_t* kb;

	if (!profile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");

	for (i = 0; i < 256; i++)
	{
		sprintf(szElement, "%03i", i);

		kb = &profile->keybindings[i];

		if (kb->binding && kb->binding[0])
			strcpy(szValue, kb->binding);
		else
			sprintf(szValue, "");

		// Extract the value from registry
		GetProfileRegKeyValue(pszName, szPath, "KB", szElement, szValue, sizeof(szValue), szValue);

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

			profile->size = size;
		}
		else
		{
			if (kb->binding)
				free(kb->binding);
			kb->binding = NULL;

			profile->size = 0;
		}
	}
}

// TODO: Implement

// Get profile cvars from registry
void LoadProfileCvars( char* pszName, profile_t* profile )
{
	char szPath[256], szValue[256];

	if (!profile || !pszName)
		return;

	sprintf(szPath, "Software\\Valve\\Half-Life\\Player Profiles");

	sprintf(szValue, "%f", profile->keybindings[255].lookstrafe);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "lookstrafe", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].lookstrafe = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].lookspring);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "lookspring", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].lookspring = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].crosshair);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "crosshair", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].crosshair = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].windowed_mouse);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "windowed_mouse", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].windowed_mouse = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].m_pitch);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "m_pitch", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].m_pitch = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].mfilter);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "mfilter", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].mfilter = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].mlook);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "mlook", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].mlook = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].joystick);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "joystick", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].joystick = atof(szValue);

	sprintf(szValue, "%f", profile->keybindings[255].sensitivity);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "sensitivity", szValue, sizeof(szPath), szValue);
	profile->keybindings[255].sensitivity = atof(szValue);

	sprintf(szValue, "%f", profile->viewsize);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "viewsize", szValue, sizeof(szPath), szValue);
	profile->viewsize = atof(szValue);

	sprintf(szValue, "%f", profile->brightness);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "brightness", szValue, sizeof(szPath), szValue);
	profile->brightness = atof(szValue);

	sprintf(szValue, "%f", profile->gamma);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "gamma", szValue, sizeof(szPath), szValue);
	profile->gamma = atof(szValue);

	sprintf(szValue, "%f", profile->bgmvolume);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "bgmvolume", szValue, sizeof(szPath), szValue);
	profile->bgmvolume = atof(szValue);

	sprintf(szValue, "%f", profile->suitvolume);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "suitvolume", szValue, sizeof(szPath), szValue);
	profile->suitvolume = atof(szValue);

	sprintf(szValue, "%f", profile->hisound);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "hisound", szValue, sizeof(szPath), szValue);
	profile->hisound = atof(szValue);

	sprintf(szValue, "%f", profile->volume);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "volume", szValue, sizeof(szPath), szValue);
	profile->volume = atof(szValue);

	sprintf(szValue, "%f", profile->a3d);
	GetProfileRegKeyValue(pszName, szPath, "CVAR", "a3d", szValue, sizeof(szPath), szValue);
	profile->a3d = atof(szValue);
	Cvar_Set("a3d", szValue);
}

