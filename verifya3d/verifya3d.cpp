/*
 * verifya3d.cpp
 *
 *
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
 * A3D Verification
 *
*/

#include <dsound.h>
#include <cguid.h>

/* need to include windows.h */
#include <windows.h>

#include "../common/a3d.h"
#include "verifya3d.h"

/* -------------------------------------------------------------------------- */

// A3D VERIFICATION

char* gFileInfoStrings[12] =
{
    "ProductName",
    "ProductVersion",
    "OriginalFilename",
    "FileDescription",
    "FileVersion",
    "CompanyName",
    "LegalCopyright",
    "LegalTrademarks",
    "InternalName",
    "PrivateBuild",
    "SpecialBuild",
    "Comments"
};

int WINAPI CheckA3DDllFileVersion( LPCSTR lptstrFilename, DWORD dwLen, LPVOID lpData )
{
	LPVOID lpBuffer;
	unsigned int puLen;
	char LangID[12];
	char path[64];
	char* fileInfo[12][2];
	int i;

	if (!GetFileVersionInfo(lptstrFilename, 0, dwLen, lpData))
	{
		// No version info
		return 0;
	}

	// Build LangID
	wsprintf(LangID, "%04X", GetUserDefaultLangID());
	lstrcat(LangID, "04B0");

	for (i = 0; i < 12; i++)
	{
		lstrcpy(path, "\\StringFileInfo\\");
		lstrcat(path, LangID);
		lstrcat(path, "\\");
		lstrcat(path, gFileInfoStrings[i]);

		fileInfo[0][i] = gFileInfoStrings[i];
		if (dwLen && VerQueryValue(lpData, path, &lpBuffer, &puLen))
		{
			fileInfo[1][i] = (char*)lpBuffer;
		}
		else
		{
			fileInfo[1][i] = "";
		}
	}

	if (fileInfo[0][1] && _stricmp(fileInfo[0][1], "Aureal Semiconductor") == 0)
		return 1;
	if (fileInfo[0][1] && _stricmp(fileInfo[0][1], "Aureal Semiconductor Inc.") == 0)
		return 1;
	if (fileInfo[1][1] && _stricmp(fileInfo[1][1], "SM Emulation") == 0)
		return 1;

	if ((!fileInfo[0][1] || strlen(fileInfo[0][1]) < 2) &&
		(!fileInfo[1][1] || strlen(fileInfo[1][1]) < 2))
	{
		return -1;
	}

	return 0;
}

// Public verification. Returns TRUE if Aureal, FALSE if clone, -1 if check could not succeed.
HRESULT WINAPI VerifyAurealA3D( void )
{
    char dir[MAX_PATH];
    char chData[1024];

    if (!GetSystemDirectory(dir, sizeof(dir)))
    {
        // No such directory.
        return A3D_FAILED;
    }

    if (dir)
    {     
        if (dir[strlen(dir) - 1] != '\\')
            strcat(dir, "\\A3D.DLL");
        else
            strcat(dir, "A3D.DLL");
    }

    return CheckA3DDllFileVersion(dir, sizeof(chData), chData);
}

/////////////////////////////////////////////////////////////////////////////
// A3dInitialize()
// ===============
//
// Description:
//		Initializes the COM and A3D libraries.
//
// Parameters:
//		None.
//
// Returns: 
//		S_OK:
//			Initialization and A3D registration succeeded.
//
//		S_FALSE 
//			The COM library is already initialized on this thread. 
//
//		E_FAIL:
//			Registration of one/both of the A3D libraries failed (A3D.DLL or
//			A3DAPI.DLL).
//
// Remarks:
//		You should only call this function once.  However, there is no harm
//		in calling it multiple times.  If you would just like to register
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI A3dInitialize( void )
{
    HRESULT hr;

    hr = CoInitialize(NULL);

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// A3dUninitialize()
// =================
//
// Description:
//		Informs COM that we are no longer using COM for this process.
//
// Parameters:
//		None.
//
// Returns: 
//		Nothing.
//
// Remarks:
//		Call at the exit of your program if you use any function in this
//		module.  Once COM in unitialized, no further COM objects can be used
//		until COM is initialized again via the CoInitalize() or
//		A3dInitialize() functions.
/////////////////////////////////////////////////////////////////////////////
void WINAPI A3dUninitialize( void )
{
    // Tell COM we're done using it.
    CoUninitialize();
}

/*****************************************************************************
*
*  A3dCreate()		Creates the A3D object. Basically uses COM
*					function CoCreateInstance() to initialize the IA3d4 inteface
*                   and properly register it.
*
*  Return:
*
*   >= 0            COM success
*
*   S_OK
*
*   < 0             COM error
*
* 	E_INVALIDARG	ppA3d is NULL.
*   E_FAIL
*
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// A3dCreate()
// ===========
//
// Description:
//		Creates and instantiates the A3D COM object.
//
// Parameters:
//		LPGUID lpGUID:
//			Pointer to the sound device GUID you would like to use.  To use
//			the Primary Sound Device, you can pass in NULL.
//
//		void **ppA3d:
//			If successful, this is where the IA3d5 interface pointer will be
//			placed.  Otherwise, it will be NULL.
//
//		IUnknown FAR *pUnkOuter:
//			Outer COM object used only for aggregation.  A3D does not use
//			aggregation currently so this value must be NULL.
//
//		DWORD dwFeatures:
//			Features requested to be used on the sound device.  See the
//			IA3DAPI.H file for a list of the feature flags.
//
// Returns: 
//		S_OK:
//			The method succeeded.
//
//		E_FAIL:
//			TODO: Explain possible reasons why the method failed.
//
//		E_INVALIDARG:
//			One of the arguments was passed an invalid value.
//
//		E_POINTER:
//			TODO: If one of the parameters is a pointer, add the reason here.
//
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI A3dCreate( LPGUID lpGUID, void** ppA3d, IUnknown FAR* pUnkOuter )
{
    HRESULT hresult;

    if (!ppA3d)
    {
        return E_INVALIDARG;
    }

    *ppA3d = NULL;

	// Initialize COM and A3D libraries for use.
	if (SUCCEEDED(hresult = A3dInitialize()))
	{
        // COM create the A3d root interface.
        hresult = CoCreateInstance(GUID_CLSID_A3dApi,		// Class ID registerd to a3dapi.dll.
                                   pUnkOuter,               // Aggregate COM object.
                                   CLSCTX_INPROC_SERVER,    // Code runs on the same Machine.
                                   GUID_IA3d,               // Interface that we want.
                                   ppA3d);                  // Pointer to the requested interface.
        
        if (FAILED(hresult))
        {
            // Fail
            return hresult;
        }

        hresult = ERROR_SUCCESS;
        if (*ppA3d)
        {
            // Initialize A3D
            if (SUCCEEDED(IA3d4_Init((LPA3D5)*ppA3d, lpGUID, 127, A3DRENDERPREFS_DEFAULT)))
            {
                return hresult;
            }
        }
	}

    if (*ppA3d)
    {
        IA3d4_Release((LPA3D5)*ppA3d);
        *ppA3d = NULL;
    }

    if (FAILED(hresult))
    {
        // Fail
        return hresult;
    }

    return A3D_FAILED;
}