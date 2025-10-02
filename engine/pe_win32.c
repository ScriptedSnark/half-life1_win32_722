// pe_win32.c

#include "quakedef.h"
#include "winquake.h"
#include "pr_dlls.h"

#define SIZE_OF_NT_SIGNATURE    sizeof(DWORD)
#define MAXRESOURCENAME     13

/* global macros to define header offsets into file */
/* offset to PE file signature                                 */
#define NTSIGNATURE(a) ((LPVOID)((BYTE*)a           +  \
            ((PIMAGE_DOS_HEADER)a)->e_lfanew))

/* DOS header identifies the NT PEFile signature dword
   the PEFILE header exists just after that dword              */
#define PEFHDROFFSET(a) ((LPVOID)((BYTE*)a          +  \
             ((PIMAGE_DOS_HEADER)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE))

/* PE optional header is immediately after PEFile header       */
#define OPTHDROFFSET(a) ((LPVOID)((BYTE*)a          +  \
             ((PIMAGE_DOS_HEADER)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE            +  \
             sizeof(IMAGE_FILE_HEADER)))

/* section headers are immediately after PE optional header    */
#define SECHDROFFSET(a) ((LPVOID)((BYTE*)a          +  \
             ((PIMAGE_DOS_HEADER)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE            +  \
             sizeof(IMAGE_FILE_HEADER)       +  \
             sizeof(IMAGE_OPTIONAL_HEADER)))


/* return file signature */
DWORD ImageFileType(
	LPVOID lpFile )
{
	/* dos file signature comes first */
	if (*(USHORT*)lpFile == IMAGE_DOS_SIGNATURE)
	{
	/* determine location of PE File header from dos header */
		if (LOWORD(*(DWORD*)NTSIGNATURE(lpFile)) == IMAGE_OS2_SIGNATURE ||
			LOWORD(*(DWORD*)NTSIGNATURE(lpFile)) == IMAGE_OS2_SIGNATURE_LE)
			return (DWORD)LOWORD(*(DWORD*)NTSIGNATURE(lpFile));

		else if (*(DWORD*)NTSIGNATURE(lpFile) == IMAGE_NT_SIGNATURE)
			return IMAGE_NT_SIGNATURE;

		else
			return IMAGE_DOS_SIGNATURE;
	}

	else
	/* unknown file type */
		return 0;
}


/* return the total number of sections in the module */
int NumOfSections(
	LPVOID lpFile )
{
	/* number os sections is indicated in file header */
	return ((int)((PIMAGE_FILE_HEADER)PEFHDROFFSET(lpFile))->NumberOfSections);
}


/* return offset to specified IMAGE_DIRECTORY entry */
LPVOID ImageDirectoryOffset(
	LPVOID lpFile,
	DWORD dwIMAGE_DIRECTORY )
{
	PIMAGE_OPTIONAL_HEADER	poh = (PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(lpFile);
	PIMAGE_SECTION_HEADER	psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET(lpFile);
	int						nSections = NumOfSections(lpFile);
	int						i = 0;
	LPVOID					VAImageDir;

	/* must be 0 thru (NumberOfRvaAndSizes-1) */
	if (dwIMAGE_DIRECTORY >= poh->NumberOfRvaAndSizes)
		return NULL;

	/* locate specific image directory's relative virtual address */
	VAImageDir = (LPVOID)poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress;

	/* locate section containing image directory */
	while (i++ < nSections)
	{
		if (psh->VirtualAddress <= (DWORD)VAImageDir &&
			psh->VirtualAddress + psh->SizeOfRawData > (DWORD)VAImageDir)
			break;
		psh++;
	}

	if (i > nSections)
		return NULL;

	/* return image import directory offset */
	return (LPVOID)(((int)lpFile + (int)VAImageDir - psh->VirtualAddress) +
					(int)psh->PointerToRawData);
}


/* retrieve names of all the sections in the file */
int GetSectionNames(
	LPVOID lpFile,
	HANDLE hHeap,
	char** pszSections )
{
	int				nSections = NumOfSections(lpFile);
	int				i, nCnt = 0;
	PIMAGE_SECTION_HEADER psh;
	char* ps;

	
	if (ImageFileType(lpFile) != IMAGE_NT_SIGNATURE ||
		(psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET(lpFile)) == NULL)
		return 0;

	/* count the number of chars used in the section names */
	for (i = 0; i < nSections; i++)
		nCnt += strlen((char*)psh[i].Name) + 1;

	/* allocate space for all section names from heap */
	ps = *pszSections = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, nCnt);

	
	for (i = 0; i < nSections; i++)
	{
		strcpy(ps, (char*)psh[i].Name);
		ps += strlen((char*)psh[i].Name) + 1;
	}

	return nCnt;
}


/* function gets the function header for a section identified by name */
BOOL GetSectionHdrByName(
	LPVOID lpFile,
	IMAGE_SECTION_HEADER* sh,
	char* szSection )
{
	PIMAGE_SECTION_HEADER psh;
	int				nSections = NumOfSections(lpFile);
	int				i;

	
	if ((psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET(lpFile)) != NULL)
	{
		/* find the section by name */
		for (i = 0; i < nSections; i++)
		{
			if (!strcmp((char*)psh->Name, szSection))
			{
				/* copy data to header */
				CopyMemory((LPVOID)sh, (LPVOID)psh, sizeof(IMAGE_SECTION_HEADER));
				return TRUE;
			}
			else
				psh++;
		}
	}

	return FALSE;
}


/* function indicates whether debug  info has been stripped from file */
BOOL IsDebugInfoStripped(
	LPVOID lpFile )
{
	PIMAGE_FILE_HEADER	pfh;
	
	pfh = (PIMAGE_FILE_HEADER)PEFHDROFFSET(lpFile);

	return (pfh->Characteristics & IMAGE_FILE_DEBUG_STRIPPED);
}


/* function looks up for the symbol table in a PE file */
char* GetExportedFunctionNames(
	LPVOID lpFile )
{
	IMAGE_SECTION_HEADER	sh;
	PIMAGE_EXPORT_DIRECTORY	ped;

	/* get section header and pointer to data directory for .edata section */
	if ((ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryOffset
		(lpFile, IMAGE_DIRECTORY_ENTRY_EXPORT)) == NULL)
		return NULL;

	/* locate section header for ".rdata" section */
	if (!GetSectionHdrByName(lpFile, &sh, ".rdata"))
		return NULL;

	/* determine the offset of the export function names */
	return (char*)(*(int*)((int)ped->AddressOfNames -
						(int)sh.VirtualAddress +
						(int)sh.PointerToRawData +
						(int)lpFile) -
					(int)sh.VirtualAddress +
					(int)sh.PointerToRawData +
					(int)lpFile);
}


/* calculate the total length of the function export names */
int GetExportsAndSize(
	LPVOID lpFile )
{
	PIMAGE_EXPORT_DIRECTORY	ped;
	char* pNames, * pCnt;
	int					i, nCnt;

	/* get section header and pointer to data directory for .edata section */
	if ((ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryOffset
		(lpFile, IMAGE_DIRECTORY_ENTRY_EXPORT)) == NULL)
		return 0;

	pNames = GetExportedFunctionNames(lpFile);
	if (!pNames)
		return 0;

	/* figure out how much memory to allocate for all strings */
	pCnt = pNames;
	for (i = 0; i < (int)ped->NumberOfNames; i++)
		pCnt += strlen(pCnt) + 1;
	nCnt = (int)(pCnt - pNames);
	
	return nCnt;
}


/* retrieve the names of all functions exported from a PE32 module and store them in a buffer */
void GetExportsAndNames(
	LPVOID lpFile, char* pszFunctions )
{
	PIMAGE_EXPORT_DIRECTORY	ped;
	char* pNames, * pCnt;
	int					i, nCnt;

	/* get section header and pointer to data directory for .edata section */
	if ((ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryOffset
		(lpFile, IMAGE_DIRECTORY_ENTRY_EXPORT)) == NULL)
		return;

	pNames = GetExportedFunctionNames(lpFile);
	if (!pNames)
		return;

	/* figure out how much memory to allocate for all strings */
	pCnt = pNames;
	for (i = 0; i < (int)ped->NumberOfNames; i++)
		pCnt += strlen(pCnt) + 1;
	nCnt = (int)(pCnt - pNames);

	/* copy all string to buffer */
	CopyMemory((LPVOID)pszFunctions, (LPVOID)pNames, nCnt);
}

// open the file, map it to memory, and parse the .rdata section for the names of the export functions
int ExportNamesSize( const char* pszFileName, char** pszFunctions )
{
	HFILE				hFile;
	struct _OFSTRUCT	pReOpenBuffer;
	HANDLE				hFileMapping;
	byte*				lpMapFileBase;
	int					nTotalLengthOfNames;

	hFile = OpenFile(pszFileName, &pReOpenBuffer, OF_READ);
	if (!hFile)
		return 0;

	hFileMapping = CreateFileMapping((HANDLE)hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hFileMapping)
	{
		// couldn't open file
		CloseHandle((HANDLE)hFile);
		return 0;
	}

	lpMapFileBase = (byte*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	if (!lpMapFileBase)
	{
		// couldn't map view of file
		CloseHandle(hFileMapping);
        CloseHandle((HANDLE)hFile);
		return 0;
	}

	// calc how much memory we need to allocate for these export function names with passing NULL output buffer
	nTotalLengthOfNames = GetExportsAndSize(lpMapFileBase);
	if (nTotalLengthOfNames)
	{
		/* allocate memory off heap for function names */
		*pszFunctions = Hunk_TempAlloc(nTotalLengthOfNames);
		GetExportsAndNames(lpMapFileBase, *pszFunctions);
	}
	else
	{
		*pszFunctions = NULL;
	}

	// cleanup handles
	UnmapViewOfFile(lpMapFileBase);
	CloseHandle(hFileMapping);
	CloseHandle((HANDLE)hFile);

	return nTotalLengthOfNames;
}

// cleanup mangled function names and copy them
void ExportNamesCopy( char* pszFunctionMangleName, char* szFunctionName )
{
	int		at = 0;
	int		c = 1; // skip first '?'

	// lookup for char '@' until second time
	while (at < 2)
	{
		szFunctionName[c - 1] = pszFunctionMangleName[c];
		if (pszFunctionMangleName[c] == '@')
			at++;

		c++;
	}
	szFunctionName[c - 2] = 0;
}

// Load the function table of an external DLL file
void BuildExportTable( extensiondll_t* pextdll, const char* pszDllFilename )
{
	int		i, j;
	int		nFunctionCount = 0;
	int		nTotalSize = 0;
	void* pMemoryBlock;
	int		nMaxFunctionNameSize = 0;
	char	szFunctionName[256];
	char* pExportFuncNames, * pFunctionNameTable, * pszCurrentMangleName;

	if (!pextdll || !pszDllFilename)
	{
		return;
	}

	nTotalSize = ExportNamesSize(pszDllFilename, &pExportFuncNames);

	pextdll->functionTable = NULL;
	pextdll->functionCount = 0;

	if (!pExportFuncNames)
	{
		return;
	}

	// Find the number of export functions in the DLL and calculate the maximum size (length) of function names
	// Allocate memory for the function table and function name table
	for (i = 0; i < nTotalSize; )
	{
		pszCurrentMangleName = &pExportFuncNames[i];
		if (pExportFuncNames[i] == '?')
		{
			nFunctionCount++;
			ExportNamesCopy(pszCurrentMangleName, szFunctionName);
			nMaxFunctionNameSize += strlen(szFunctionName) + 1;
		}

		i += strlen(pszCurrentMangleName) + 1;
	}

	pMemoryBlock = malloc(nFunctionCount * sizeof(functiontable_t) + nMaxFunctionNameSize * sizeof(char));
	pextdll->functionTable = pMemoryBlock;
	pextdll->functionCount = nFunctionCount;

	// Take pointer after allocated to functiontable_t for function name table
	pFunctionNameTable = (char*)pMemoryBlock + (nFunctionCount * sizeof(functiontable_t));

	// Populate the function table with function name pointers and function pointers
	// obtained from GetProcAddress
	for (i = 0, j = 0; i < nTotalSize; )
	{
		pszCurrentMangleName = &pExportFuncNames[i];
		if (pExportFuncNames[i] == '?')
		{
			ExportNamesCopy(&pExportFuncNames[i], pFunctionNameTable);
			pextdll->functionTable[j].pFunctionName = pFunctionNameTable;
            pextdll->functionTable[j].pFunction = (uint32)GetProcAddress((HMODULE)pextdll->lDLLHandle, pszCurrentMangleName);
			j++;

			pFunctionNameTable += strlen(pFunctionNameTable) + 1;
		}

		i += strlen(pszCurrentMangleName) + 1;
	}
}