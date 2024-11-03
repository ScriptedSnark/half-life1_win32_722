// Customization.h

#ifndef INC_CUSTOMIZATION
#define INC_CUSTOMIZATION

#define MAX_QPATH 64    // Must match value in quakedefs.h

/////////////////
// Customization
// passed to pfnPlayerCustomization
// For automatic downloading.
typedef enum
{
	t_sound,
	t_skin,
	t_model,
	t_decal,
} resourcetype_t;

#define RES_FATALIFMISSING (1<<0)   // Disconnect if we can't get this file.
#define RES_WASMISSING     (1<<1)   // Do we have the file locally, did we get it ok?
#define RES_CUSTOM         (1<<2)   // Is this resource one that corresponds to another player's customization
									//  or is it a server startup resource.
// MD5 Hash
typedef struct
{
	unsigned int buf[4];
	unsigned int bits[2];
	unsigned char in[64];
} MD5Context_t;

typedef struct resource_s
{
	char              szFileName[MAX_QPATH]; // File name to download/precache.
	resourcetype_t    type;                // t_sound, t_skin, t_model, t_decal.
	int               nIndex;              // For t_decals
	int               nDownloadSize;       // Size in Bytes if this must be downloaded.
	unsigned char     ucFlags;

// For handling client to client resource propagation
	unsigned char     rgucMD5_hash[16];    // To determine if we already have it.
	unsigned char     playernum;           // Which player index this resource is associated with, if it's a custom resource.

	struct resource_s* pNext;              // Next in chain.
	struct resource_s* pPrev;
} resource_t;

typedef struct customization_s
{
	qboolean bInUse;     // Is this customization in use;
	resource_t resource; // The resource_t for this customization
	qboolean bTranslated; // Has the raw data been translated into a useable format?  
						  //  (e.g., raw decal .wad make into texture_t *)
	int        nUserData1; // Customization specific data
	int        nUserData2; // Customization specific data
	void* pInfo;          // Buffer that holds the data structure that references the data (e.g., the cachewad_t)
	void* pBuffer;       // Buffer that holds the data for the customization (the raw .wad data)
	struct customization_s* pNext; // Next in chain
} customization_t;

#endif // !Customization