#include "platform.h"
#include "quakedef.h"
#include "sys.h"
#include <dlfcn.h>

#define HIDDEN __attribute__ ((visibility ("hidden")))

HIDDEN farproc_t Sys_GetProcAddress(dllhandle_t handle, const char* name)
{
#ifdef _WIN32
	return GetProcAddress(handle, name);
#else
	return dlsym(handle, name);
#endif
}

HIDDEN dllhandle_t Sys_LoadLibrary(const char* name)
{
#ifdef _WIN32
	return LoadLibraryA(name);
#else
	dllhandle_t pLibrary;
	if (!(pLibrary = dlopen(name, RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL)))
	{
		Sys_Error("Failed to load library %s: %s", name, dlerror());
		return NULL;
	}

	return pLibrary;
#endif
}

HIDDEN void Sys_FreeLibrary(dllhandle_t handle)
{
#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}