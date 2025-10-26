#include "launcher.h"

#include <SDL2/SDL_messagebox.h>

void Sys_Sleep(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); // Convert milliseconds to microseconds
#endif
}

void Sys_Error(const char* error, ...)
{
    va_list argptr;
    char text[1024];

    va_start(argptr, error);
    vsnprintf(text, sizeof(text), error, argptr);
    va_end(argptr);

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", text, NULL);
	exit(1);
}

/*
================
Sys_GetProcAddress
================
*/
farproc_t Sys_GetProcAddress(dllhandle_t handle, const char* name)
{
#ifdef _WIN32
	return GetProcAddress(handle, name);
#else
	return dlsym(handle, name);
#endif
}

dllhandle_t Sys_LoadLibrary(const char* name)
{
#ifdef _WIN32
	return LoadLibraryA(name);
#else
	dllhandle_t pLibrary;
	if (!(pLibrary = dlopen(name, RTLD_NOW | RTLD_GLOBAL)))
	{
		Sys_Error("Failed to load library %s: %s", name, dlerror());
		return NULL;
	}

	return pLibrary;
#endif
}

#ifdef _WIN32
/*
================
Sys_GetProfileRegKeyValue

Gets profile settings from the registry
================
*/
void Sys_GetProfileRegKeyValue(char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszReturnString, int nReturnLength, char* pszDefaultValue)
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
		// Didn't find it, so write out new value
		else
		{
			// Just Set the Values according to the defaults
			lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (const BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
		}
	}

	RegCloseKey(hKey);
}

/*
================
Sys_SetProfileRegKeyValue

Sets a profile value in the registry
================
*/
void Sys_SetProfileRegKeyValue(char* pszName, char* pszPath, char* pszSetting, char* pszElement, char* pszDefaultValue)
{
	HKEY hKey;
	DWORD dwDisposition;
	LONG lResult;
	char szSubKey[256];

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

	// Set the value
	lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (const BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);

	RegCloseKey(hKey);
}
#endif

char* Sys_GetCommandLine(void)
{
    return sys_cmdline;
}
