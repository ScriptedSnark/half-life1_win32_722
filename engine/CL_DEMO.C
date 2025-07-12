// cl_demo.c

#include "quakedef.h"
#include "tmessage.h"
#include "cl_demo.h"

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

	// Close the file
	fclose(cls.demofile);
	cls.demofile = NULL;
	fclose(fp);

	// Replace original file
	Con_Printf("Replacing old demo with edited version.\n");
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	// Release entry info
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

	// Close the file
	fclose(cls.demofile);
	cls.demofile = NULL;
	fclose(fp);

	// Replace original file
	Con_Printf("Replacing old demo with edited version.\n");
	_unlink(szOriginalName);
	rename(szTempName, szOriginalName);

	// Release entry info
	free(demodir.p_rgEntries);
	demodir.p_rgEntries = NULL;

	demodir.nEntries = 0;
}





//=============================================== FINISH LINE END



// TODO: Implement

/*
====================
CL_WriteDemoUpdate

====================
*/
void CL_WriteDemoUpdate( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
====================
CL_RecordHUDCommand

====================
*/
void CL_RecordHUDCommand( char* cmdname )
{
	// TODO: Implement
}

/*
====================
CL_WriteDLLUpdate

====================
*/
void CL_WriteDLLUpdate( client_data_t* cdata )
{
	// TODO: Implement
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
CL_StopPlayback

Called when a demo file runs out, or the user starts a game
====================
*/
void CL_StopPlayback( void )
{
	// TODO: Implement
}




/*
====================
CL_Stop_f

stop recording a demo
====================
*/
void CL_Stop_f( void )
{
	// TODO: Implement
}

/*
====================
CL_Record_f

//
====================
*/
void CL_Record_f( void )
{
	// TODO: Implement
}

// TODO: Implement