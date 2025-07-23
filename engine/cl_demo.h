// cl_demo.h
#ifndef CL_DEMO_H
#define CL_DEMO_H
#ifdef _WIN32
#pragma once
#endif

// NOTE:  Change this any time the file version of the demo changes
#define DEMO_PROTOCOL 4

// Demo lump types
#define DEMO_STARTUP		0	// This lump contains startup info needed to spawn into the server
#define DEMO_NORMAL			1	// This lump contains playback info of messages, etc., needed during playback.

// unknown message
#define dem_unknown			0
// it's a startup message, process as fast as possible
#define dem_norewind		1
// move the demostart time value forward by this amount
#define dem_jumptime		2
// sequence info
#define dem_sequenceinfo	3
// update entities
#define dem_updateents		4
// HUD message
#define dem_hud				5
// userdata from the client.dll
#define dem_userdata		6
// demo header
#define dem_header			7
// it's a normal network packet
#define dem_stop			8

//
// Structures
//

// header info
typedef struct demoheader_s
{
	char szFileStamp[8];		// Should be HLDEMO
	int nDemoProtocol;			// Should be DEMO_PROTOCOL
	int nNetProtocolVersion;	// Should be PROTOCOL_VERSION
	char szMapName[MAX_OSPATH];	// Name of map
	char szDllDir[MAX_OSPATH];	// Name of game directory (com_gamedir)
	CRC32_t mapCRC;
	int nDirectoryOffset;		// Offset of Entry Directory.
} demoheader_t;

typedef struct demoentry_s
{
	int nEntryType;				// DEMO_STARTUP or DEMO_NORMAL
	char szDescription[64];
	int nFlags;
	int nCDTrack;
	float fTrackTime;			// Time of track
	int nFrames;				// # of frames in track
	int nOffset;				// File offset of track data
	int nFileLength;			// Length of track
} demoentry_t;

typedef struct demodirectory_s
{
	// Number of tracks
	int nEntries;
	// Track entry info
	demoentry_t* p_rgEntries;
} demodirectory_t;

extern client_textmessage_t tm_demomessage;

extern cvar_t	cl_appendmixed;

void COM_CopyFileChunk( FILE* dst, FILE* src, int nSize );

void CL_AppendDemo_f( void );
void CL_SwapDemo_f( void );
void CL_SetDemoInfo_f( void );
void CL_RemoveDemo_f( void );
void CL_ListDemo_f( void );
void CL_Stop_f( void );
void CL_Record_f( void );
void CL_PlayDemo_f( void );
void CL_TimeDemo_f( void );

void CL_BeginDemoStartup( void );
void CL_WriteDemoStartup( sizebuf_t* msg );
void CL_WriteDemoHeader( char* filename );
void CL_WriteDemoMessage( sizebuf_t* msg );
void CL_WriteDLLUpdate( void );
void CL_RecordHUDCommand( char* cmdname );
void CL_DemoUpdateClientData( client_data_t* cdata );
void CL_StopPlayback( void );

qboolean CL_ReadDemoMessage( void );
qboolean CL_ReadDemoViewInfo( float* origin, float* angles );

#endif // CL_DEMO_H