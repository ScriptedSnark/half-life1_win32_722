#include "launcher.h"

qboolean CheckParm(const char* pszParm, char** ppszValue)
{
    static char* argv[64];
    static char argStrings[2048];
    static int argc = 0;
    static qboolean parsed = FALSE;
    
    if (!parsed)
    {
        char* start = argStrings;
        char* end;
        int len;
        
        strcpy(argStrings, sys_cmdline);
        
        start = argStrings;
        while (*start && argc < (sizeof(argv) / sizeof(argv[0]) - 1))
        {
            while (*start == ' ') start++;
            if (!*start) break;

            end = start;
            while (*end && *end != ' ') end++;
            
            argv[argc++] = start;
            
            if (*end)
            {
                *end = '\0';
                start = end + 1;
            }
            else
                break;
        }

        argv[argc] = NULL;
        parsed = TRUE;
    }
    
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], pszParm))
        {
            if (ppszValue && (i + 1) < argc)
            {
                if (argv[i + 1][0] != '-')
                {
                    *ppszValue = argv[i + 1];
                }
                else
                {
                    *ppszValue = NULL;
                }
            }
            return TRUE;
        }
    }
    
    if (ppszValue)
        *ppszValue = NULL;

    return FALSE;
}

qboolean CheckParmEx(const char* pszParm)
{
    return CheckParm(pszParm, NULL);
}