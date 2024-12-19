// cl_parse.c  -- parse a message received from the server

#include "quakedef.h"


int		last_data[64];
int		msg_buckets[64];


/*
================
CL_RequestMissingResources

================
*/
qboolean CL_RequestMissingResources( void )
{
	// TODO: Implement
	return FALSE;
}


/*
=================
CL_Parse_ServerInfo

Read in server info packet.
=================
*/
void CL_ParseServerInfo( void )
{
	Con_DPrintf("Serverinfo packet received.\n");

	// TODO: Implement
}





int	total_data[64];




#define SHOWNET(x) \
	if (cl_shownet.value == 2.0 && Q_strlen(x) > 1) \
		Con_Printf("%3i:%s\n", msg_readcount - 1, x);

/*
=================
CL_ParseServerMessage

Parse incoming message from server.
=================
*/
void CL_ParseServerMessage( void )
{
	// Index of svc_ or user command to issue.
	int cmd;
	// For determining data parse sizes
	int bufStart, bufEnd;

//
// if recording demos, copy the message out
//
	if (cl_shownet.value == 1.0)
	{
		Con_Printf("%i ", net_message.cursize);
	}
	else if (cl_shownet.value == 2.0)
	{
		Con_Printf("------------------\n");
	}

	cl.onground = 0;	// unless the server says otherwise

	memset(last_data, 0, sizeof(last_data));

	while (1)
	{
		if (msg_badread)
			Host_Error("CL_ParseServerMessage: Bad server message");

		// Mark start position
		bufStart = msg_readcount;

		cmd = MSG_ReadByte();

		// Bogus message?
		if (cmd == -1)
			break;

		if (cmd > svc_lastmsg)
		{
			// TODO: Implement

			continue;
		}
		
		switch (cmd)
		{
		default:
			// TODO: Implement
			break;

		case svc_nop:
//			Con_Printf("svc_nop\n");
			break;

		// TODO: Implement

		case svc_print:
			Con_Printf("%s", MSG_ReadString());
			break;

		case svc_stufftext:
			Cbuf_AddText(MSG_ReadString());
			break;

		// TODO: Implement

		case svc_serverinfo:
			CL_ParseServerInfo();
			vid.recalc_refdef = TRUE;	// leave intermission full screen
			break;

		// TODO: Implement
		}

		// TODO: Implement
	}

	// end of message
	SHOWNET("END OF MESSAGE");

	// TODO: Implement
}