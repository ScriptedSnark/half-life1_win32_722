// cl_demo.c

#include "quakedef.h"
#include "tmessage.h"
#include "cl_demo.h"
#include "shake.h"
#include "view.h"

void CL_FinishTimeDemo( void );
void CL_HudMessage( const char* pMessage );

/*
==============================================================================

DEMO CODE

When a demo is playing back, all NET_SendMessages are skipped, and
NET_GetMessages are read from the demo file.

Whenever cl.time gets past the last received message, another message is
read from the demo file.
==============================================================================
*/

// Demo flags
#define FDEMO_TITLE			0x01	// Show title
#define FDEMO_CDTRACK		0x04	// Playing cd track
#define FDEMO_FADE_IN_SLOW	0x08	// Fade in (slow)
#define FDEMO_FADE_IN_FAST	0x10	// Fade in (fast)
#define FDEMO_FADE_OUT_SLOW	0x20	// Fade out (slow)
#define FDEMO_FADE_OUT_FAST	0x40	// Fade out (fast)

ScreenFade sf_demo;
qboolean bInFadeout = FALSE;

cvar_t	cl_appendmixed = { "cl_appendmixed", "0" };

char	gDemoMessageBuffer[4];

client_textmessage_t tm_demomessage =
{
	0, // effect
	255, 255, 255, 255,
	255, 255, 255, 255,
	-1.0f, // x
	-1.0f, // y
	0.0f, // fadein
	0.0f, // fadeout
	0.0f, // holdtime
	0.0f, // fxTime,
	DEMO_MESSAGE,  // pName message name.
	gDemoMessageBuffer   // pMessage
};

// Demo file i/o stuff
demoentry_t* pCurrentEntry;
int nCurEntryIndex;

demodirectory_t demodir = { 0, NULL };
demoheader_t demoheader;

float CL_DemoOutTime( void )
{
	return realtime;
}

float CL_DemoInTime( void )
{
	return Sys_FloatTime();
}

/*
====================
CL_AppendDemo_f

Record a demo, appending to the demo file already at half-life/valve/demo.dem
====================
*/
void CL_AppendDemo_f( void )
{
	char    name[MAX_OSPATH];
	int		track;
	int		c;
	byte	cmd;
	float	f;
	FILE* fp;
	int		copysize;
	char	szTempName[MAX_OSPATH];
	char	szMapName[MAX_OSPATH];
	int		frame;
	int		swlen;

	if (cmd_source != src_command)
		return;

	if (cls.demorecording || cls.demoplayback)
	{
		Con_Printf("Appenddemo only available when not running or recording a demo.\n");
		return;
	}

	if (cls.state != ca_active)
	{
		Con_Printf("You must be in an active map spawned on a machine that allows creation of files in %s\n", com_gamedir);
		return;
	}

	c = Cmd_Argc();
	if (c != 2 && c != 3)
	{
		Con_Printf("appenddemo <demoname> <cd track>\n");
		return;
	}

	if (strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	track = -1;
	if (c == 3)
	{
		track = atoi(Cmd_Argv(2));
		Con_Printf("Forcing CD track to %i\n", track);
	}

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");

	Con_Printf("Appending to demo %s.\n", name);
	strcpy(cls.demofilename, name);
	fp = fopen(name, "rb");
	if (!fp)
	{
		Con_Printf("Error:  couldn't open demo file %s for appending.\n", name);
		return;
	}

	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".dm2");

	cls.demofile = fopen(szTempName, "w+b");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Ready to start reading
	// Now copy the stuff we cached from the server.
	fseek(fp, 0, SEEK_END);
	copysize = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);

	COM_CopyFileChunk(cls.demofile, fp, copysize);

	// Close the file
	fclose(fp);

	fseek(cls.demofile, 0, SEEK_SET);
	// Read in the demoheader
	fread(&demoheader, sizeof(demoheader), 1, cls.demofile);

	if (strcmp(demoheader.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		_unlink(szTempName);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	COM_FileBase(cl.worldmodel->name, szMapName);
	if (strcmp(demoheader.szMapName, szMapName))
	{
		Con_Printf("%s was recorded on a different map, cannot append.\n", name);
		_unlink(szTempName);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	if (!cl_appendmixed.value && cl.serverCRC != demoheader.mapCRC)
	{
		Con_Printf("Map CRC's mismatched, cannot append.\n");
		Con_Printf("Set 'cl_appendmixed' to 1 to allow appending at your own risk.\n");
		_unlink(szTempName);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	if (demoheader.nNetProtocolVersion != PROTOCOL_VERSION ||
		demoheader.nDemoProtocol != DEMO_PROTOCOL)
	{
		Con_Printf(
			"ERROR: demo protocol outdated\n"
			"Demo file protocols %iN:%iD\n"
			"Server protocol is at %iN:%iD\n",
			demoheader.nNetProtocolVersion,
			demoheader.nDemoProtocol,
			PROTOCOL_VERSION,
			DEMO_PROTOCOL
		);
		_unlink(szTempName);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	// Now read in the directory structure.
	fseek(cls.demofile, demoheader.nDirectoryOffset, SEEK_SET);
	fread(&demodir.nEntries, sizeof(int), 1, cls.demofile);

	if (demodir.nEntries < 1 ||
		demodir.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			demodir.nEntries);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * (demodir.nEntries + 1));
	fread(demodir.p_rgEntries, sizeof(demoentry_t), demodir.nEntries, cls.demofile);

	nCurEntryIndex = demodir.nEntries++;
	// now we are writing the first real lump
	pCurrentEntry = &demodir.p_rgEntries[nCurEntryIndex]; // First real data lump

	memset(pCurrentEntry, 0, sizeof(demoentry_t));

	fseek(cls.demofile, demoheader.nDirectoryOffset, SEEK_SET);

	pCurrentEntry->nEntryType = DEMO_NORMAL;
	sprintf(pCurrentEntry->szDescription, "Playback '%i'", demodir.nEntries - 1);

	pCurrentEntry->nFlags = 0;
	pCurrentEntry->fTrackTime = 0.0f;
	pCurrentEntry->nCDTrack = track;

	pCurrentEntry->nOffset = ftell(cls.demofile);  // Position for this chunk.

	cls.demostarttime = CL_DemoOutTime();	// setup the demo starttime
	cls.demoframecount = 0;
	cls.demostartframe = host_framecount;

	// Demo playback should read this as an incoming message.
	// Write the client's realtime value out so we can synchronize the reads.
	cmd = dem_jumptime;
	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	// Time offset
	f = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&f, sizeof(f), 1, cls.demofile);

	frame = host_framecount - cls.demostartframe;
	swlen = LittleLong(frame);
	fwrite(&swlen, sizeof(swlen), 1, cls.demofile);

	// don't start saving messages until a non-delta compressed message is received
	cls.demowaiting = TRUE;
	cls.demorecording = TRUE;
	cls.demoappending = TRUE;

	cls.td_startframe = host_framecount;	
	cls.td_lastframe = -1;		// get a new message this frame

	// Close down the hud for now.
	ClientDLL_HudReset();

	Cbuf_InsertText("impulse 204\n");
	Cbuf_Execute();
}

/*
===============
COM_CopyFileChunk

===============
*/
void COM_CopyFileChunk( FILE* dst, FILE* src, int nSize )
{
	int   copysize = nSize;
	char  copybuf[COM_COPY_CHUNK_SIZE];

	while (copysize > COM_COPY_CHUNK_SIZE)
	{
		fread(copybuf, COM_COPY_CHUNK_SIZE, 1, src);
		fwrite(copybuf, COM_COPY_CHUNK_SIZE, 1, dst);
		copysize -= COM_COPY_CHUNK_SIZE;
	}

	fread(copybuf, copysize, 1, src);
	fwrite(copybuf, copysize, 1, dst);

	fflush(src);
	fflush(dst);
}

/*
====================
CL_SwapDemo_f

Swap two segments (positions) in a demo
====================
*/
void CL_SwapDemo_f( void )
{
	char	name[MAX_OSPATH];
	int		c;
	FILE* fp;
	char	szTempName[MAX_OSPATH];
	char	szOriginalName[MAX_OSPATH];
	int		nSegment1;
	int		nSegment2;
	demoentry_t* oldentry, * newentry;
	demoentry_t temp;
	int		n, i;

	if (cmd_source != src_command)
		return;

	if (cls.demorecording || cls.demoplayback)
	{
		Con_Printf("Swapdemo only available when not running or recording a demo.\n");
		return;
	}

	c = Cmd_Argc();
	if (c != 4)
	{
		Con_Printf("Swapdemo <demoname> <seg#> <seg#>\nSwaps segments, segment 1 cannot be moved\n");
		return;
	}

	if (strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	nSegment1 = atoi(Cmd_Argv(2));
	if (nSegment1 <= 1)
	{
		Con_Printf("Cannot swap the STARTUP segment.\n");
		return;
	}

	nSegment2 = atoi(Cmd_Argv(3));

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");

	Con_Printf("Swapping segment %i for %i from demo %s.\n", nSegment1, nSegment2, name);

	strcpy(szOriginalName, name);
	fp = fopen(szOriginalName, "rb");
	if (!fp)
	{
		Con_Printf("Error:  couldn't open demo file %s for swapping.\n", name);
		return;
	}

	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".dm2");

	cls.demofile = fopen(szTempName, "w+b");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Ready to start reading
	// Now copy the stuff we cached from the server.
	fseek(fp, 0, SEEK_END);
	n = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);

	COM_CopyFileChunk(cls.demofile, fp, n);

	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);
	// Read in the demoheader
	fread(&demoheader, sizeof(demoheader), 1, fp);

	if (strcmp(demoheader.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	if (demoheader.nNetProtocolVersion != PROTOCOL_VERSION ||
		demoheader.nDemoProtocol != DEMO_PROTOCOL)
	{
		Con_Printf(
			"ERROR: demo protocol outdated\n"
			"Demo file protocols %iN:%iD\n"
			"Server protocol is at %iN:%iD\n",
			demoheader.nNetProtocolVersion,
			demoheader.nDemoProtocol,
			PROTOCOL_VERSION,
			DEMO_PROTOCOL
		);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	// Now read in the directory structure.
	fseek(fp, demoheader.nDirectoryOffset, SEEK_SET);

	fread(&demodir.nEntries, sizeof(int), 1, fp);

	if (demodir.nEntries < 1 ||
		demodir.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			demodir.nEntries);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * demodir.nEntries);
	fread(demodir.p_rgEntries, sizeof(demoentry_t), demodir.nEntries, fp);

	// Swap these 2 segments
	oldentry = &demodir.p_rgEntries[nSegment1 - 1];
	memcpy(&temp, oldentry, sizeof(temp));

	newentry = &demodir.p_rgEntries[nSegment2 - 1];
	memcpy(oldentry, newentry, sizeof(*oldentry));
	memcpy(newentry, &temp, sizeof(*newentry));

	fseek(cls.demofile, demoheader.nDirectoryOffset, SEEK_SET);

	fwrite(&demodir.nEntries, sizeof(int), 1, cls.demofile);
	for (i = 0; i < demodir.nEntries; i++)
	{
		fwrite(&demodir.p_rgEntries[i], sizeof(demoentry_t), 1, cls.demofile);
	}

	fclose(cls.demofile);
	cls.demofile = NULL;
	fclose(fp);

	// Replace original file
	Con_Printf("Replacing old demo with edited version.\n");
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;
	demodir.nEntries = 0;
}

/*
====================
CL_SetDemoInfo_f

Add info to demo: info = title "text", play tracknum, fade <IN|OUT><FAST|SLOW>
====================
*/
void CL_SetDemoInfo_f( void )
{
	char	name[MAX_OSPATH];
	int		c;
	FILE* fp;
	char	szTempName[MAX_OSPATH];
	char	szOriginalName[MAX_OSPATH];
	int		nSegment;
	demoentry_t* newentry;
	int		n, i;
	qboolean bDone;
	int		nCurArg;
	char* pFlag;
	char* pValue;
	char* pDirection;

	if (cmd_source != src_command)
		return;

	if (cls.demorecording || cls.demoplayback)
	{
		Con_Printf("Setdemoinfo only available when not running or recording a demo.\n");
		return;
	}

	c = Cmd_Argc();
	if (c < 3)
	{
		Con_Printf("Setdemoinfo <demoname> <seg#> <info ...>\n");
		Con_Printf("   title \"text\"\n");
		Con_Printf("   play tracknum\n");
		Con_Printf("   fade <in | out> <fast | slow>\n\n");
		Con_Printf("Use -option to disable, e.g., -title\n");
		return;
	}

	if (strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	nSegment = atoi(Cmd_Argv(2));
	if (nSegment <= 1)
	{
		Con_Printf("Cannot Setdemoinfo the STARTUP segment.\n");
		return;
	}

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");
	Con_Printf("Setting info for segment %i in demo %s.\n", nSegment, name);

	strcpy(szOriginalName, name);
	fp = fopen(szOriginalName, "rb");
	if (!fp)
	{
		Con_Printf("Error:  couldn't open demo file %s for Setdemoinfo.\n", name);
		return;
	}

	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".dm2");

	cls.demofile = fopen(szTempName, "w+b");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Ready to start reading
	// Now copy the stuff we cached from the server.
	fseek(fp, 0, SEEK_END);

	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);

	// Copy the chunk
	COM_CopyFileChunk(cls.demofile, fp, n);

	// Read in the demofile
	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);

	// Read in the demoheader
	fread(&demoheader, sizeof(demoheader), 1, fp);

	if (strcmp(demoheader.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	if (demoheader.nNetProtocolVersion != PROTOCOL_VERSION ||
		demoheader.nDemoProtocol != DEMO_PROTOCOL)
	{
		Con_Printf(
			"ERROR: demo protocol outdated\n"
			"Demo file protocols %iN:%iD\n"
			"Server protocol is at %iN:%iD\n",
			demoheader.nNetProtocolVersion,
			demoheader.nDemoProtocol,
			PROTOCOL_VERSION,
			DEMO_PROTOCOL
		);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	// Now read in the directory structure
	fseek(fp, demoheader.nDirectoryOffset, SEEK_SET);

	fread(&demodir.nEntries, sizeof(int), 1, fp);

	if (demodir.nEntries < 1 ||
		demodir.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			demodir.nEntries);
		fclose(cls.demofile);
		fclose(fp);
		_unlink(szTempName);
		cls.demofile = NULL;
		return;
	}

	// Read in the entry info structure
	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * demodir.nEntries);
	fread(demodir.p_rgEntries, sizeof(demoentry_t), demodir.nEntries, fp);

	nCurArg = 3;
	newentry = &demodir.p_rgEntries[nSegment - 1];

	bDone = FALSE;
	while (!bDone)
	{
		pFlag = Cmd_Argv(nCurArg);
		if (!pFlag || !pFlag[0])
			break;

		nCurArg++;

		if (!_stricmp(pFlag, "-TITLE"))
		{
			newentry->nFlags &= ~FDEMO_TITLE;
		}
		else if (!_stricmp(pFlag, "-PLAY"))
		{
			newentry->nFlags &= ~FDEMO_CDTRACK;
		}
		else if (!_stricmp(pFlag, "-FADE"))
		{
			newentry->nFlags &= ~(FDEMO_FADE_IN_SLOW | FDEMO_FADE_IN_FAST | FDEMO_FADE_OUT_SLOW | FDEMO_FADE_OUT_FAST);
		}
		else if (!_stricmp(pFlag, "TITLE"))
		{
			pValue = Cmd_Argv(nCurArg);
			nCurArg++;

			if (!pValue || !pValue[0])
			{
				Con_Printf("Title flag requires a double-quoted value.\n");
				continue;
			}

			strncpy(newentry->szDescription, pValue, sizeof(newentry->szDescription) - 1);
			newentry->szDescription[sizeof(newentry->szDescription) - 1] = 0;
			newentry->nFlags |= FDEMO_TITLE;
		}
		else if (!_stricmp(pFlag, "PLAY"))
		{
			pValue = Cmd_Argv(nCurArg);
			nCurArg++;

			if (!pValue || !pValue[0])
			{
				Con_Printf("Play flag requires a cd track #.\n");
				continue;
			}

			newentry->nCDTrack = atoi(pValue);
			newentry->nFlags |= FDEMO_CDTRACK;
		}
		else if (!_stricmp(pFlag, "FADE"))
		{
			pDirection = Cmd_Argv(nCurArg);
			nCurArg++;

			if (!pDirection || !pDirection[0])
			{
				Con_Printf("Fade flag requires a direction and speed (in fast, e.g.)\n");
				continue;
			}

			if (!_stricmp(pDirection, "IN"))
			{
				pValue = Cmd_Argv(nCurArg);
				nCurArg++;

				if (!pValue || !pValue[0])
				{
					Con_Printf("Fade flag requires a speed (fast or slow).\n");
					continue;
				}

				if (!_stricmp(pValue, "FAST"))
				{
					newentry->nFlags |= FDEMO_FADE_IN_FAST;
				}
				else if (!_stricmp(pValue, "SLOW"))
				{
					newentry->nFlags |= FDEMO_FADE_IN_SLOW;
				}
				else
				{
					Con_Printf("Fade flag requires a speed (fast or slow).\n");
					continue;
				}
			}
			else if (!_stricmp(pDirection, "OUT"))
			{
				pValue = Cmd_Argv(nCurArg);
				nCurArg++;

				if (!pValue || !pValue[0])
				{
					Con_Printf("Fade flag requires a speed (fast or slow).\n");
					continue;
				}

				if (!_stricmp(pValue, "FAST"))
				{
					newentry->nFlags |= FDEMO_FADE_OUT_FAST;
				}
				else if (!_stricmp(pValue, "SLOW"))
				{
					newentry->nFlags |= FDEMO_FADE_OUT_SLOW;
				}
				else
				{
					Con_Printf("Fade flag requires a speed (fast or slow).\n");
					continue;
				}
			}
			else
			{
				Con_Printf("Fade flag requires a direction (in or out).\n");
				continue;
			}
		}
		else
		{
			Con_Printf("Setdemoinfo, unrecognized flag:  %s\n", pFlag);
			bDone = TRUE;
		}
	}

	fseek(cls.demofile, demoheader.nDirectoryOffset, SEEK_SET);

	// Read in the directory structure.
	fwrite(&demodir.nEntries, sizeof(int), 1, cls.demofile);
	for (i = 0; i < demodir.nEntries; i++)
	{
		fwrite(&demodir.p_rgEntries[i], sizeof(demoentry_t), 1, cls.demofile);
	}

	fclose(cls.demofile);
	cls.demofile = NULL;
	fclose(fp);

	// Replace original file
	Con_Printf("Replacing old demo with edited version.\n");
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;
	demodir.nEntries = 0;
}

/*
====================
CL_RemoveDemo_f

Remove segment from a demo
====================
*/
void CL_RemoveDemo_f( void )
{
	char	name[MAX_OSPATH];
	int		c;
	FILE* fp;
	char	szTempName[MAX_OSPATH];
	char	szOriginalName[MAX_OSPATH];
	int		nSegmentToRemove;
	demodirectory_t newdir;
	demoentry_t* oldentry, * newentry;
	int		n, i;

	if (cmd_source != src_command)
		return;

	if (cls.demorecording || cls.demoplayback)
	{
		Con_Printf("Removedemo only available when not running or recording a demo.\n");
		return;
	}

	c = Cmd_Argc();
	if (c != 3)
	{
		Con_Printf("removedemo <demoname> <segment to remove>\nSegment 1 cannot be removed\n");
		return;
	}

	if (strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	nSegmentToRemove = atoi(Cmd_Argv(2));
	if (nSegmentToRemove <= 1)
	{
		Con_Printf("Cannot remove the STARTUP segment.\n");
		return;
	}

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");
	Con_Printf("Removing segment %i from demo %s.\n", nSegmentToRemove, name);

	strcpy(szOriginalName, name);
	fp = fopen(szOriginalName, "rb");
	if (!fp)
	{
		Con_Printf("Error:  couldn't open demo file %s for segment removal.\n", name);
		return;
	}

	COM_StripExtension(name, szTempName);
	COM_DefaultExtension(szTempName, ".dm2");

	cls.demofile = fopen(szTempName, "w+b");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open %s.\n", name);
		return;
	}

	// Read in the filepath and in the demofile
	// as well
	fseek(fp, 0, SEEK_SET);
	fseek(cls.demofile, 0, SEEK_SET);

	// Read in the demoheader
	fread(&demoheader, sizeof(demoheader), 1, fp);

	// Write in the demofile
	fwrite(&demoheader, sizeof(demoheader), 1, cls.demofile);

	if (strcmp(demoheader.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		fclose(cls.demofile);
		fclose(fp);
		cls.demofile = NULL;
		return;
	}

	if (demoheader.nNetProtocolVersion != PROTOCOL_VERSION ||
		demoheader.nDemoProtocol != DEMO_PROTOCOL)
	{
		Con_Printf(
			"ERROR: demo protocol outdated\n"
			"Demo file protocols %iN:%iD\n"
			"Server protocol is at %iN:%iD\n",
			demoheader.nNetProtocolVersion,
			demoheader.nDemoProtocol,
			PROTOCOL_VERSION,
			DEMO_PROTOCOL
		);
		fclose(cls.demofile);
		fclose(fp);
		cls.demofile = NULL;
		return;
	}

	// Now read in the directory structure
	fseek(fp, demoheader.nDirectoryOffset, SEEK_SET);

	fread(&demodir.nEntries, sizeof(int), 1, fp);

	if (demodir.nEntries < 1 ||
		demodir.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			demodir.nEntries);
		fclose(cls.demofile);
		fclose(fp);
		cls.demofile = NULL;
		return;
	}

	if (demodir.nEntries == 1)
	{
		Con_Printf("ERROR: Can't remove last directory entry.\n");
		fclose(cls.demofile);
		fclose(fp);
		cls.demofile = NULL;
		return;
	}

	// Read in the entry info structure
	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * demodir.nEntries);
	fread(demodir.p_rgEntries, sizeof(demoentry_t), demodir.nEntries, fp);

	newdir.nEntries = demodir.nEntries - 1;
	newdir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * newdir.nEntries);

	n = nSegmentToRemove - 1;
	for (i = 0; i < demodir.nEntries; i++)
	{
		if (i == n)
			continue;

		oldentry = &demodir.p_rgEntries[i];
		newentry = &newdir.p_rgEntries[i];
		memcpy(&newdir.p_rgEntries[i], &demodir.p_rgEntries[i], sizeof(demoentry_t));

		newentry->nOffset = ftell(cls.demofile);
		fseek(fp, oldentry->nOffset, SEEK_SET);
		COM_CopyFileChunk(cls.demofile, fp, newentry->nFileLength);
	}

	demoheader.nDirectoryOffset = ftell(cls.demofile);

	// Read in the directory structure.
	fwrite(&newdir.nEntries, sizeof(int), 1, cls.demofile);

	for (i = 0; i < newdir.nEntries; i++)
	{
		fwrite(&newdir.p_rgEntries[i], sizeof(demoentry_t), 1, cls.demofile);
	}

	// Read in the demofile
	fseek(cls.demofile, 0, SEEK_SET);
	fwrite(&demoheader, sizeof(demoheader), 1, cls.demofile);

	fclose(cls.demofile);
	cls.demofile = NULL;
	fclose(fp);

	// Replace original file
	Con_Printf("Replacing old demo with edited version.\n");
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	free(newdir.p_rgEntries);
	free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;
	demodir.nEntries = 0;
}

/*
===============
CL_ListDemo_f

List the contents of a demo file.
===============
*/
void CL_ListDemo_f( void )
{
	int	nCurrent;
	demoheader_t header;
	demodirectory_t directory;
	demoentry_t* entry;
	char	name[MAX_OSPATH];
	char	type[32];

	if (cmd_source != src_command)
		return;

	if (cls.demorecording || cls.demoplayback)
	{
		Con_Printf("Listdemo only available when not running or recording.\n");
		return;
	}

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");
	Con_Printf("Demo contents for %s.\n", name);

	cls.demofile = fopen(name, "rb");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	// Read in the header
	fread(&header, sizeof(header), 1, cls.demofile);

	if (strcmp(header.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	Con_Printf("Demo file protocols %iN:%iD\nServer protocols at %iN:%iD\n",
		header.nNetProtocolVersion,
		header.nDemoProtocol,
		PROTOCOL_VERSION,
		DEMO_PROTOCOL
	);

	Con_Printf("Map name :  %s\n", header.szMapName);
	Con_Printf("Game .dll:  %s\n", header.szDllDir);

	// Now read in the directory structure.
	fseek(cls.demofile, header.nDirectoryOffset, SEEK_SET);

	fread(&directory.nEntries, sizeof(int), 1, cls.demofile);

	if (directory.nEntries < 1 ||
		directory.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			directory.nEntries);
		fclose(cls.demofile);
		cls.demofile = NULL;
		return;
	}

	directory.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * directory.nEntries);
	fread(directory.p_rgEntries, sizeof(demoentry_t), directory.nEntries, cls.demofile);

	Con_Printf("\n");

	for (nCurrent = 0; nCurrent < directory.nEntries; nCurrent++)
	{
		entry = &directory.p_rgEntries[nCurrent];

		if (entry->nEntryType == DEMO_STARTUP)
			sprintf(type, "Start segment");
		else
			sprintf(type, "Normal segment");

		Con_Printf("%i:  %s\n  \"%20s\"\nTime:  %.2f s.\nFrames:  %i\nSize:  %.2fK\n",
			nCurrent + 1, type, entry->szDescription, entry->fTrackTime, entry->nFrames, ((float)entry->nFileLength / 1024.0));

		if (entry->nFlags)
			Con_Printf("  Flags:\n");
		if (entry->nFlags & FDEMO_TITLE)
			Con_Printf("     Title \"%s\"\n", entry->szDescription);
		if (entry->nFlags & FDEMO_CDTRACK)
			Con_Printf("     Play %i\n", entry->nCDTrack);
		if (entry->nFlags & FDEMO_FADE_IN_SLOW)
			Con_Printf("     Fade in slow\n");
		if (entry->nFlags & FDEMO_FADE_IN_FAST)
			Con_Printf("     Fade in fast\n");
		if (entry->nFlags & FDEMO_FADE_OUT_SLOW)
			Con_Printf("     Fade out slow\n");
		if (entry->nFlags & FDEMO_FADE_OUT_FAST)
			Con_Printf("     Fade out fast\n");

		Con_Printf("\n");
	}

	fclose(cls.demofile);
	cls.demofile = NULL;

	free(directory.p_rgEntries);
}

/*
===============
CL_DemoSectionStart

Start section, ñalled for each demo at the very beginning
Apply fade/sound effects if needed
===============
*/
void CL_DemoSectionStart( void )
{
	char szCommand[128];

	if (!pCurrentEntry)
		return;

	if (pCurrentEntry->nFlags & FDEMO_CDTRACK)
	{
		sprintf(szCommand, "cd stop\ncd play %i\n", pCurrentEntry->nCDTrack);
		Cbuf_AddText(szCommand);
	}

	if (pCurrentEntry->nFlags & FDEMO_FADE_IN_FAST)
	{
		sf_demo.duration = 2048;
		sf_demo.holdTime = 0;
		sf_demo.fadeFlags = 0;
		sf_demo.r = 0;
		sf_demo.g = 0;
		sf_demo.b = 0;
		sf_demo.a = 255;
		V_ScreenFade(NULL, 0, &sf_demo);
	}

	if (pCurrentEntry->nFlags & FDEMO_FADE_IN_SLOW)
	{
		sf_demo.duration = 8192;
		sf_demo.holdTime = 0;
		sf_demo.fadeFlags = 0;
		sf_demo.r = 0;
		sf_demo.g = 0;
		sf_demo.b = 0;
		sf_demo.a = 255;
		V_ScreenFade(NULL, 0, &sf_demo);
	}
}

/*
===============
CL_SwitchToNextDemoSection

Switch to next section
===============
*/
qboolean CL_SwitchToNextDemoSection( void )
{
	float ft;

	if (nCurEntryIndex >= 1)
	{
		if (pCurrentEntry && (pCurrentEntry->nFlags & FDEMO_CDTRACK))
		{
			Cbuf_AddText("cd stop\n");
		}
	}

	if (++nCurEntryIndex >= demodir.nEntries)
	{
		ft = CL_DemoInTime();

		if (cls.demonorewinds != 0)
			cls.demototaltimediff /= cls.demonorewinds;

		// Done
		Host_EndGame("End of demo.\n");
	}

	// Switch to next section, we got a dem_stop
	pCurrentEntry = &demodir.p_rgEntries[nCurEntryIndex];

	cls.forcetrack = pCurrentEntry->nCDTrack;

	// Ready to continue reading, reset clock.
	fseek(cls.demofile, pCurrentEntry->nOffset, SEEK_SET);

	// Time is now relative to this chunk's clock.
	ft = CL_DemoInTime();
	cls.demoframecount = 0;
	cls.demostarttime = ft;
	cls.demostartframe = host_framecount;
	return TRUE;
}

/*
====================
CL_WriteDLLUpdate

====================
*/
void CL_WriteDLLUpdate( void )
{
	byte	cmd;
	float	time;
	int		frame;

	cmd = dem_userdata;
	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demofile);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demofile);
}

/*
====================
CL_DemoWriteSequenceInfo

Save state of cls.netchan sequences so that
we can play the demo correctly.
====================
*/
void CL_DemoWriteSequenceInfo( void )
{
	byte	cmd;
	float	time;
	int		frame;

	cmd = dem_sequenceinfo;
	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demofile);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demofile);

	fwrite(&cls.netchan.incoming_sequence, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.incoming_acknowledged, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.incoming_reliable_acknowledged, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.incoming_reliable_sequence, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.outgoing_sequence, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.reliable_sequence, sizeof(int), 1, cls.demofile);
	fwrite(&cls.netchan.last_reliable_sequence, sizeof(int), 1, cls.demofile);
}

/*
====================
CL_DemoReadNetchanState

Read netchan data written in a demofile.
====================
*/
void CL_DemoReadNetchanState( void )
{
	fread(&cls.netchan.incoming_sequence, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.incoming_acknowledged, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.incoming_reliable_acknowledged, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.incoming_reliable_sequence, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.outgoing_sequence, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.reliable_sequence, sizeof(int), 1, cls.demofile);
	fread(&cls.netchan.last_reliable_sequence, sizeof(int), 1, cls.demofile);
}

/*
====================
CL_DemoWriteCmdInfo

====================
*/
void CL_DemoWriteCmdInfo( FILE* fp )
{
	int		i;
	float	value;

	for (i = 0; i < 3; i++)
	{
		value = LittleFloat(cl.viewangles[i]);
		fwrite(&value, sizeof(value), 1, fp);
		value = LittleFloat(cl.simorg[i]);
		fwrite(&value, sizeof(value), 1, fp);
	}
}

/*
====================
CL_DemoReadCmdInfo

====================
*/
void CL_DemoReadCmdInfo( void )
{
	int		i;
	float	value;

	for (i = 0; i < 3; i++)
	{
		fread(&value, sizeof(value), 1, cls.demofile);
		cl.viewangles[i] = LittleFloat(value);
		fread(&value, sizeof(value), 1, cls.demofile);
		cl.simorg[i] = LittleFloat(value);
	}
}

/*
====================
CL_RecordHUDCommand

====================
*/
void CL_RecordHUDCommand( char* cmdname )
{
	byte	cmd;
	float	time;
	int		frame;
	char	szNameBuf[64];

	cmd = dem_hud;
	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demofile);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demofile);

	// Write the name of the message
	memset(szNameBuf, 0, sizeof(szNameBuf));
	strcpy(szNameBuf, cmdname);
	fwrite(szNameBuf, sizeof(szNameBuf), 1, cls.demofile);
}

/*
====================
CL_DemoUpdateClientData

====================
*/
void CL_DemoUpdateClientData( client_data_t* cdata )
{
	byte	cmd;
	float	time;
	int		frame;

	if (!cls.demofile)
		return;

	cmd = dem_userdata;
	fwrite(&cmd, sizeof(cmd), 1, cls.demofile);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demofile);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demofile);

	fwrite(cdata, sizeof(client_data_t), 1, cls.demofile);
}

/*
====================
CL_BeginDemoStartup
====================
*/
void CL_BeginDemoStartup( void )
{
	if (cls.demoheader)
		fclose(cls.demoheader);

	cls.demoheader = tmpfile();
	if (!cls.demoheader)
	{
		Con_DPrintf("ERROR: couldn't open temporary header file.\n");
		return;
	}
	Con_DPrintf("Spooling demo header.\n");
}

/*
====================
CL_WriteDemoStartup
====================
*/
void CL_WriteDemoStartup( sizebuf_t* msg )
{
	byte	cmd;
	float	time;
	int		frame;
	int		len, start;
	int		curpos;

	if (!cls.demoheader)
		return;

	len = msg->cursize;
	start = LittleLong(len);
	if (len <= 0)
		return;

	curpos = ftell(cls.demoheader);

	cmd = dem_unknown;
	fwrite(&cmd, sizeof(cmd), 1, cls.demoheader);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demoheader);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demoheader);

	// Store current info
	CL_DemoWriteCmdInfo(cls.demoheader);

	// Output the buffer.
	fwrite(&start, sizeof(start), 1, cls.demoheader);
	fwrite(msg->data, len, 1, cls.demoheader);
}

/*
====================
CL_WriteDemoHeader
====================
*/
void CL_WriteDemoHeader( char* filename )
{
	byte	cmd;
	float	time;
	int		frame;
	int		len;
	char	szFileName[MAX_OSPATH];

	if (cls.state != ca_connected && cls.state != ca_uninitialized)
		return;

	if (cls.demoplayback || !cls.demoheader)
		return;

	cmd = dem_header;
	fwrite(&cmd, sizeof(cmd), 1, cls.demoheader);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(time), 1, cls.demoheader);

	// And frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(frame), 1, cls.demoheader);

	memset(szFileName, 0, sizeof(szFileName));

	if (filename)
	{
		len = strlen(filename);
		if (len >= sizeof(szFileName))
			len = sizeof(szFileName) - 1;

		memcpy(szFileName, filename, len);
		szFileName[len] = 0;
	}

	fwrite(szFileName, sizeof(szFileName), 1, cls.demoheader);
}

/*
====================
CL_StopPlayback

Called when a demo file runs out, or the user starts a game
====================
*/
void CL_StopPlayback( void )
{
	if (!cls.demoplayback)
		return;

	fclose(cls.demofile);
	cls.demofile = NULL;

	cls.demoplayback = FALSE;

	cls.demoframecount = 0;
	cls.state = ca_disconnected;

	if (demodir.p_rgEntries)
		free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;
	demodir.nEntries = 0;

	if (cls.timedemo)
		CL_FinishTimeDemo();

	if (pCurrentEntry->nFlags & FDEMO_CDTRACK)
		Cbuf_AddText("cd stop\n");
}

/*
====================
CL_ReadDemoMessage

Parse the demo message
====================
*/
qboolean CL_ReadDemoMessage( void )
{
	int		msglen;
	byte	msgbuf[MAX_MSGLEN];
	int		r;
	float	f;
	float	ft;
	byte	cmd;
	int		curpos;
	vec3_t	origin, viewangles;

	int		frame;
	qboolean bSkipMessage;
	qboolean bDoneReading;

	msglen = 0;

	cls.demomaxdelayexceeded = FALSE;

	if (!cls.demofile)
	{
		cls.demoplayback = FALSE;
		CL_Disconnect_f();
		Con_Printf("Tried to read a demo message with no demo file\n");
		return FALSE;
	}

	bDoneReading = FALSE;
	do
	{
		curpos = ftell(cls.demofile);
		net_message.cursize = 0;

		// Read the command
		r = fread(&cmd, sizeof(byte), 1, cls.demofile);

		// Read the timestamp
		r = fread(&f, sizeof(float), 1, cls.demofile);
		f = LittleFloat(f);

		// Read the framestamp
		r = fread(&frame, sizeof(int), 1, cls.demofile);
		frame = LittleLong(frame);

		ft = CL_DemoInTime() - cls.demostarttime;

		// Time demo ignores clocks and tries to synchronize frames to what was recorded
		//  I.e., while frame is the same, read messages, otherwise, skip out.
		if (!cls.timedemo)
		{
			bSkipMessage = (f >= ft) ? TRUE : FALSE;
		}
		else
		{
			bSkipMessage = (frame > (host_framecount - cls.demostartframe)) ? TRUE : FALSE;
		}
		
		if (cmd != dem_unknown &&   // Startup messages don't require looking at the timer.
			cmd != dem_stop &&
			bSkipMessage)   // Not ready for a message yet, put it back on the file.
		{
			curpos = ftell(cls.demofile);
			fseek(cls.demofile, curpos - 9, SEEK_SET);
			cls.demoskippedmessages++;
			return FALSE;
		}

		if (cmd == dem_norewind)
		{
			cls.demonorewinds++;
			cls.demototaltimediff += ft - f;
		}

		cls.democurrenttimediff = ft - f;
		if (!cls.timedemo && cmd && cls.democurrenttimediff >= 0.1)
			cls.demomaxdelayexceeded = TRUE;

		// COMMAND HANDLERS
		switch (cmd)
		{
		case dem_jumptime:
			{
				cls.demostarttime = CL_DemoInTime();
				cls.demototaltimediff = 0.0;
				cls.demostartframe = host_framecount;
				cls.demonorewinds = 0;
			}
			break;
		case dem_stop:
			{
				if (!CL_SwitchToNextDemoSection())
					bDoneReading = TRUE;
			}
			break;
		case dem_sequenceinfo:
			{
				CL_DemoReadNetchanState();
			}
			break;
		case dem_updateents:
			{
				cls.demoupdateentities = TRUE;
			}
			break;
		case dem_hud:
			{
				char szCmdName[64];
				fread(szCmdName, sizeof(szCmdName), 1, cls.demofile);

				Cbuf_AddText(szCmdName);
				Cbuf_AddText("\n");
			}
			break;
		case dem_userdata:
			{
				client_data_t cdat;
				fread(&cdat, sizeof(client_data_t), 1, cls.demofile);
				ClientDLL_DemoUpdateClientData(&cdat);
			}
			break;
		case dem_header:
			{
				char szFileName[128];
				fread(szFileName, sizeof(szFileName), 1, cls.demofile);
				strcpy(cls.dl.szFileName, szFileName);
			}
			break;
		}
	} while (!bDoneReading);

	if (nCurEntryIndex)
	{
		if (cls.demoframecount == 0)
		{
			// This is a start section
			CL_DemoSectionStart();
		}

		if (nCurEntryIndex && cls.demoframecount == 1)
		{
			bInFadeout = FALSE;
			if (pCurrentEntry->nFlags & FDEMO_TITLE)
			{
				SetDemoMessage(pCurrentEntry->szDescription, 0.01, 1.0, 2.0);
				CL_HudMessage(DEMO_MESSAGE);
			}

			if (!cls.timedemo)
				cls.demostarttime = CL_DemoInTime();
		}
	}

	cls.demoframecount++;

	if (nCurEntryIndex && !bInFadeout)
	{
		if (pCurrentEntry->nFlags & FDEMO_FADE_OUT_FAST)
		{
			if (!bInFadeout && pCurrentEntry->fTrackTime + 0.02 <= ft + 0.5)
			{
				bInFadeout = TRUE;
				sf_demo.r = 0;
				sf_demo.g = 0;
				sf_demo.b = 0;
				sf_demo.a = 255;
				sf_demo.duration = 2048;
				sf_demo.holdTime = 409;
				sf_demo.fadeFlags = FFADE_OUT;
				V_ScreenFade(NULL, 0, &sf_demo);
			}
		}

		if (pCurrentEntry->nFlags & FDEMO_FADE_OUT_SLOW)
		{
			if (!bInFadeout && pCurrentEntry->fTrackTime + 0.02 <= ft + 2.0)
			{
				bInFadeout = TRUE;
				sf_demo.r = 0;
				sf_demo.g = 0;
				sf_demo.b = 0;
				sf_demo.a = 255;
				sf_demo.duration = 8192;
				sf_demo.holdTime = 409;
				sf_demo.fadeFlags = FFADE_OUT;
				V_ScreenFade(NULL, 0, &sf_demo);
			}
		}
	}

	CL_DemoReadCmdInfo();

	r = fread(&msglen, sizeof(int), 1, cls.demofile);
	if (r != 1)
	{
		Host_EndGame("Bad demo length.\n");
	}

	msglen = LittleLong(msglen);

	if (msglen < 0)
	{
		Host_EndGame("Demo message length < 0.\n");
	}

	if (msglen < 8)
	{
		Con_DPrintf("read runt demo message\n");
	}

	if (msglen > MAX_MSGLEN)
	{
		Host_EndGame("Demo message > MAX_MSGLEN");
	}

	if (msglen > 0)
	{
		r = fread(msgbuf, msglen, 1, cls.demofile);
		if (r != 1)
		{
			Host_EndGame("Error reading demo message data.\n");
		}
	}

	// Simulate receipt in the message queue
	memcpy(net_message.data, msgbuf, msglen);
	net_message.cursize = msglen;

	if (CL_ReadDemoViewInfo(origin, viewangles))
	{
		VectorCopy(origin, cl.simorg);
		VectorCopy(viewangles, cl.viewangles);
	}

	return TRUE;
}

/*
====================
CL_ReadDemoViewInfo

====================
*/
qboolean CL_ReadDemoViewInfo( float* origin, float* angles )
{
	int		i;
	int		r;
	float	f;
	byte	cmd;
	int		startpos;
	int		curpos;

	int		frame;
	qboolean bDoneReading;

	if (!cls.demofile)
		return FALSE;

	startpos = ftell(cls.demofile);

	bDoneReading = FALSE;
	do
	{
		curpos = ftell(cls.demofile);

		// Read the command
		r = fread(&cmd, sizeof(byte), 1, cls.demofile);

		// Read the timestamp
		r = fread(&f, sizeof(float), 1, cls.demofile);
		f = LittleFloat(f);

		// Read the framestamp
		r = fread(&frame, sizeof(int), 1, cls.demofile);
		frame = LittleLong(frame);

		if (cmd == dem_jumptime)
			continue;

		if (cmd == dem_stop)
		{
			fseek(cls.demofile, startpos, SEEK_SET);
			return FALSE;
		}

		// COMMAND HANDLERS
		switch (cmd)
		{
		case dem_sequenceinfo:
			{
				int rgSequenceInfo[7];
				fread(rgSequenceInfo, sizeof(rgSequenceInfo), 1, cls.demofile);
			}
			break;
		case dem_updateents:
			break;
		case dem_hud:
			{
				char szCmdName[64];
				fread(szCmdName, sizeof(szCmdName), 1, cls.demofile);
			}
			break;
		case dem_userdata:
			{
				client_data_t cdat;
				fread(&cdat, sizeof(client_data_t), 1, cls.demofile);
			}
			break;
		case dem_header:
			{
				char szFileName[128];
				fread(szFileName, sizeof(szFileName), 1, cls.demofile);
			}
			break;
		default:
			bDoneReading = TRUE;
			break;
		}
	} while (!bDoneReading);

	for (i = 0; i < 3; i++)
	{
		fread(&f, sizeof(f), 1, cls.demofile);
		angles[i] = LittleFloat(f);
		fread(&f, sizeof(f), 1, cls.demofile);
		origin[i] = LittleFloat(f);
	}

	fseek(cls.demofile, startpos, SEEK_SET);
	return TRUE;
}

/*
====================
CL_WriteDemoMessage

Dumps the current net message, prefixed by the length and view angles
====================
*/
void CL_WriteDemoMessage( sizebuf_t* msg )
{
	byte c;
	float time;
	int	frame;
	int	len, swlen;
	int	curpos;

	if (!cls.demorecording || !cls.demofile)
		return;

	len = msg->cursize;
	swlen = LittleLong(len);

	if (len < 0)
		return;

	curpos = ftell(cls.demofile);

	cls.demoframecount++;

	CL_DemoWriteSequenceInfo();

	// Demo playback should read this as an incoming message.
	c = dem_norewind;
	fwrite(&c, sizeof(c), 1, cls.demofile);

	// Write time
	time = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&time, sizeof(float), 1, cls.demofile);

	// Write frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(int), 1, cls.demofile);

	// Store current info
	CL_DemoWriteCmdInfo(cls.demofile);

	// Write the length out.
	fwrite(&swlen, sizeof(int), 1, cls.demofile);

	// Output the buffer.
	fwrite(msg->data, len, 1, cls.demofile);
}

/*
====================
CL_Stop_f

stop recording a demo
====================
*/
void CL_Stop_f( void )
{
	byte	c;
	float	f;
	float	ft;
	int		frame;
	int		curpos;
	int		i;
	char	szNewName[MAX_OSPATH];
	char	szOldName[MAX_OSPATH];

	if (cmd_source != src_command)
		return;

	if (!cls.demorecording)
	{
		Con_Printf("Not recording a demo.\n");
		return;
	}

	c = dem_stop;
	fwrite(&c, sizeof(byte), 1, cls.demofile);

	// Write time
	ft = CL_DemoOutTime() - cls.demostarttime;
	f = LittleFloat(ft);
	fwrite(&f, sizeof(float), 1, cls.demofile);

	// Write frame number
	frame = LittleLong(host_framecount - cls.demostartframe);
	fwrite(&frame, sizeof(int), 1, cls.demofile);

	// Close down the hud for now.
	ClientDLL_HudReset();

	curpos = ftell(cls.demofile);

	pCurrentEntry->nFileLength = curpos - pCurrentEntry->nOffset;
	pCurrentEntry->fTrackTime = ft;
	pCurrentEntry->nFrames = cls.demoframecount;

	//  Now write out the directory and free it and touch up the demo header.
	fwrite(&demodir.nEntries, sizeof(int), 1, cls.demofile);
	for (i = 0; i < demodir.nEntries; i++)
	{
		fwrite(&demodir.p_rgEntries[i], sizeof(demoentry_t), 1, cls.demofile);
	}

	if (demodir.p_rgEntries)
		free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;
	demodir.nEntries = 0;

	demoheader.nDirectoryOffset = curpos;

	// Write in the demofile
	fseek(cls.demofile, 0, SEEK_SET);
	fwrite(&demoheader, sizeof(demoheader_t), 1, cls.demofile);

	fclose(cls.demofile); // finish up
	cls.demofile = NULL;
	cls.demorecording = FALSE;

	cls.td_lastframe = host_framecount;

	if (cls.demoappending)
	{
		Con_Printf("Replacing old demo with appended version.\n");
		strcpy(szNewName, cls.demofilename);
		_unlink(cls.demofilename);

		COM_StripExtension(cls.demofilename, szOldName);
		COM_DefaultExtension(szOldName, ".dm2");

		rename(szOldName, szNewName);
	}

	Con_Printf("Completed demo\n");
	Con_DPrintf("Recording time %.2f\nHost frames %i\n", ft, cls.td_lastframe - cls.td_startframe);
}

/*
====================
CL_Record_f

record <demoname> <server>
====================
*/
void CL_Record_f( void )
{
	int		c;
	char	name[MAX_OSPATH];
	char	szMapName[MAX_OSPATH];
	char	szDll[MAX_OSPATH];
	int		track;
	int		copysize;
	int		savepos;
	int		curpos;
	byte	cmd;
	float	f;
	int		frame;
	int		swlen;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("record <demoname> <cd>\n");
		return;
	}

	if (cls.demorecording)
	{
		Con_Printf("Already recording.\n");
		return;
	}

	if (cls.demoplayback)
	{
		Con_Printf("Can't record during demo playback.\n");
		return;
	}

	if (!cls.demoheader ||
		(cls.state != ca_active))
	{
		Con_Printf("You must be in an active map spawned on a machine that allows creation of files in %s\n", com_gamedir);
		return;
	}

	c = Cmd_Argc();
	if (c != 2 && c != 3)
	{
		Con_Printf("record <demoname> <cd track>\n");
		return;
	}

	if (strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}

	track = -1;
	if (c == 3)
	{
		track = atoi(Cmd_Argv(2));
		Con_Printf("Forcing CD track to %i\n", track);
	}

//
// open the demo file
//
	sprintf(name, "%s/%s", com_gamedir, Cmd_Argv(1));

	COM_DefaultExtension(name, ".dem");
	Con_Printf("recording to %s.\n", name);

	cls.demofile = fopen(name, "wb");
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	memset(&demoheader, 0, sizeof(demoheader));
	strcpy(demoheader.szFileStamp, "HLDEMO");

	COM_FileBase(cl.worldmodel->name, szMapName);
	strcpy(demoheader.szMapName, szMapName);

	strcpy(szDll, com_gamedir);
	COM_FileBase(szDll, demoheader.szDllDir);

	demoheader.mapCRC = cl.serverCRC;
	demoheader.nDemoProtocol = DEMO_PROTOCOL;
	demoheader.nNetProtocolVersion = PROTOCOL_VERSION;
	demoheader.nDirectoryOffset = 0;  // Temporary.  We rewrite the structure at the end.

	// Demos must match current server/client protocol.
	fwrite(&demoheader, sizeof(demoheader_t), 1, cls.demofile);

	memset(&demodir, 0, sizeof(demodir));
	demodir.nEntries = 2;
	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * demodir.nEntries);
	memset(demodir.p_rgEntries, 0, sizeof(demoentry_t) * demodir.nEntries);

	// DIRECTORY ENTRY # 0
	pCurrentEntry = &demodir.p_rgEntries[0]; // Only one here.
	pCurrentEntry->nEntryType = DEMO_STARTUP;
	strcpy(pCurrentEntry->szDescription, "LOADING");
	pCurrentEntry->nFlags = 0;
	pCurrentEntry->nCDTrack = track;
	pCurrentEntry->fTrackTime = 0.0f; // Startup takes 0 time.
	pCurrentEntry->nOffset = ftell(cls.demofile);  // Position for this chunk.

	// don't start saving messages until a non-delta compressed message is received
	cls.demowaiting = TRUE;
	cls.demorecording = TRUE;

	curpos = ftell(cls.demofile);

	// Finish off the startup info.
	cmd = dem_stop;
	fwrite(&cmd, sizeof(byte), 1, cls.demoheader);

	f = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&f, sizeof(float), 1, cls.demoheader);

	frame = host_framecount - cls.demostartframe;
	swlen = LittleLong(frame);
	fwrite(&swlen, sizeof(int), 1, cls.demoheader);

	fflush(cls.demoheader);

	// Now copy the stuff we cached from the server.
	copysize = ftell(cls.demoheader);
	savepos = copysize;

	fseek(cls.demoheader, 0, SEEK_SET);

	COM_CopyFileChunk(cls.demofile, cls.demoheader, copysize);

	// Jump back to end, in case we record another demo for this session.
	fseek(cls.demoheader, savepos, SEEK_SET);

	cls.demostarttime = CL_DemoOutTime();
	cls.demoframecount = 0;
	cls.demostartframe = host_framecount;

	// Now move on to entry # 2, the first data chunk.
	curpos = ftell(cls.demofile);

	pCurrentEntry->nFileLength = curpos - pCurrentEntry->nOffset;

// DIRECTORY ENTRY # 1

	// Now we are writing the first real lump.
	pCurrentEntry = &demodir.p_rgEntries[1]; // First real data lump
	pCurrentEntry->nEntryType = DEMO_NORMAL;
	sprintf(pCurrentEntry->szDescription, "Playback");
	pCurrentEntry->nFlags = 0;
	pCurrentEntry->nCDTrack = track;
	pCurrentEntry->fTrackTime = 0.0f; // Startup takes 0 time.
	pCurrentEntry->nOffset = ftell(cls.demofile);  // Position for this chunk.

	// Demo playback should read this as an incoming message.
	// Write the client's realtime value out so we can synchronize the reads.
	cmd = dem_jumptime;
	fwrite(&cmd, sizeof(byte), 1, cls.demofile);

	f = LittleFloat(CL_DemoOutTime() - cls.demostarttime);
	fwrite(&f, sizeof(float), 1, cls.demofile);

	frame = host_framecount - cls.demostartframe;
	swlen = LittleLong(frame);
	fwrite(&swlen, sizeof(int), 1, cls.demofile);

	cls.demoappending = FALSE;

	cls.td_startframe = host_framecount;
	cls.td_lastframe = -1;		// get a new message this frame

	ClientDLL_HudReset();

	Cbuf_InsertText("impulse 204\n");
	Cbuf_Execute();
}

/*
====================
CL_PlayDemo_f

play [demoname]
====================
*/
void CL_PlayDemo_f( void )
{
	char	name[256];
	char	szDll[MAX_OSPATH];

	if (cmd_source != src_command)
		return;

	if (Cmd_Argc() != 2)
	{
		Con_Printf("play <demoname> : plays a demo\n");
		return;
	}

//
// disconnect from server
//
	CL_Disconnect();

//
// open the demo file
//
	strcpy(name, Cmd_Argv(1));
	COM_DefaultExtension(name, ".dem");

	Con_Printf("Playing demo from %s.\n", name);
	COM_FOpenFile(name, &cls.demofile);
	if (!cls.demofile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		cls.demonum = -1;		// stop demo loop
		return;
	}

	// Read in the demoheader
	fread(&demoheader, sizeof(demoheader_t), 1, cls.demofile);

	if (strcmp(demoheader.szFileStamp, "HLDEMO"))
	{
		Con_Printf("%s is not a demo file\n", name);
		fclose(cls.demofile);
		cls.demofile = NULL;
		cls.demonum = -1;		// stop demo loop
		return;
	}

	if (demoheader.nNetProtocolVersion != PROTOCOL_VERSION ||
		demoheader.nDemoProtocol != DEMO_PROTOCOL)
	{
		Con_Printf(
			"ERROR: demo protocol outdated\nDemo file protocols %iN:%iD\nServer protocol is at %iN:%D\n",
			demoheader.nNetProtocolVersion,
			demoheader.nDemoProtocol,
			PROTOCOL_VERSION,
			DEMO_PROTOCOL
		);
		fclose(cls.demofile);
		cls.demofile = NULL;
		cls.demonum = -1;		// stop demo loop
		return;
	}

	Cmd_ExecuteString(va("map %s", demoheader.szMapName), src_command);

	if (!sv.active)
	{
		Con_Printf("Couldn't start server...");
		fclose(cls.demofile);
		cls.demofile = NULL;
		cls.demonum = -1;		// stop demo loop
		return;
	}

	COM_FileBase(com_gamedir, szDll);
	if (_stricmp(demoheader.szDllDir, szDll))
	{
		Con_Printf("ERROR: demo was recorded in game:  %s, current game is %s.\n", demoheader.szDllDir, szDll);
		fclose(cls.demofile);
		cls.demofile = NULL;
		cls.demonum = -1;		// stop demo loop
		CL_Disconnect();
		return;
	}

	// Now read in the directory structure
	fseek(cls.demofile, demoheader.nDirectoryOffset, SEEK_SET);

	fread(&demodir.nEntries, sizeof(int), 1, cls.demofile);

	if (demodir.nEntries < 1 ||
		demodir.nEntries > 1024)
	{
		Con_Printf("ERROR: demo had bogus # of directory entries:  %i\n",
			demodir.nEntries);
		fclose(cls.demofile);
		cls.demofile = NULL;
		cls.demonum = -1;		// stop demo loop
		CL_Disconnect();
		return;
	}

	demodir.p_rgEntries = (demoentry_t*)malloc(sizeof(demoentry_t) * demodir.nEntries);
	fread(demodir.p_rgEntries, sizeof(demoentry_t), demodir.nEntries, cls.demofile);

	nCurEntryIndex = 0;
	pCurrentEntry = &demodir.p_rgEntries[nCurEntryIndex];

	fseek(cls.demofile, demodir.p_rgEntries->nOffset, SEEK_SET);

	cls.forcetrack = pCurrentEntry->nCDTrack;

	cls.demoplayback = TRUE;
	cls.state = ca_connected;

	cls.demostarttime = CL_DemoInTime();  // For determining whether to read another message
	cls.demostartframe = host_framecount;

	Netchan_Setup(NS_CLIENT, &cls.netchan, net_from);

	cls.demototaltimediff = 0.0f;
	cls.demoskippedmessages = 0;
	cls.demonorewinds = 0;
	cls.democurrenttimediff = 0.0f;

	cls.demoframecount = 0;
}

/*
====================
CL_FinishTimeDemo

====================
*/
void CL_FinishTimeDemo( void )
{
	int		frames;
	float	time;

	cls.timedemo = FALSE;

// the first frame didn't count
	frames = (host_framecount - cls.td_startframe - 1);
	time = realtime - cls.td_starttime;
	if (!time)
		time = 1;
	Con_Printf("%i frames %5.3f seconds %5.3f fps\n", frames, time, frames / time);
}

/*
====================
CL_TimeDemo_f

timedemo [demoname]
====================
*/
void CL_TimeDemo_f( void )
{
	if (cmd_source != src_command)
		return;

	if (Cmd_Argc() != 2)
	{
		Con_Printf("timedemo <demoname> : gets demo speeds\n");
		return;
	}

	CL_PlayDemo_f();

// cls.td_starttime will be grabbed at the second frame of the demo, so
// all the loading time doesn't get counted

	cls.timedemo = TRUE;
	cls.td_starttime = realtime;
	cls.td_startframe = host_framecount;
	cls.td_lastframe = -1;		// get a new message this frame
}