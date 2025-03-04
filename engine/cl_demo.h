// cl_demo.h
#ifndef CL_DEMO_H
#define CL_DEMO_H
#ifdef _WIN32
#pragma once
#endif


//
// Structures
//

// header info
typedef struct demoheader_s
{
	char szFileStamp[8];		// Should be HLDEMO
	int nDemoProtocol;			// Should be DEMO_PROTOCOL
	int nNetProtocolVersion;	// Should be PROTOCOL_VERSION
	char szMapName[128];		// Name of map
	char szDllDir[128];			// Name of game directory (com_gamedir)
	CRC32_t mapCRC;
	int nDirectoryOffset;		// Offset of Entry Directory.
} demoheader_t;


extern client_textmessage_t tm_demomessage;


void CL_StopPlayback( void );
void CL_WriteDemoUpdate( void );
void CL_WriteDLLUpdate( client_data_t* cdata );



void CL_Stop_f( void );
void CL_Record_f( void );



void CL_BeginDemoStartup( void );
void CL_RecordHUDCommand( char* cmdname );

#endif // CL_DEMO_H