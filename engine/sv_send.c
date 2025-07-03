// sv_main.c -- server main program

#include "quakedef.h"
#include "cmodel.h"

/*
==================
SV_SendUserReg
==================
*/
void SV_SendUserReg( sizebuf_t* sb )
{
	UserMsg* pMsg;

	for (pMsg = sv_gpNewUserMsgs; pMsg; pMsg = pMsg->next)
	{
		MSG_WriteByte(sb, svc_newusermsg);
		MSG_WriteByte(sb, pMsg->iMsg);
		MSG_WriteByte(sb, pMsg->iSize);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[0]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[4]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[8]);
		MSG_WriteLong(sb, (int)pMsg->next);
	}
}

/*
=================
SV_SendBan
=================
*/
void SV_SendBan( void )
{
	// TODO: Implement
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( void )
{
	// TODO: Implement

	return FALSE;
}
