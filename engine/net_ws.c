// net_ws.c
// Windows IP Support layer.

#include "quakedef.h"
#include "pr_cmds.h"

#if defined( _WIN32 )

#include <winsock.h>
#include <WSipx.h> // For ipx socket

typedef int socklen_t;

#endif

cvar_t host_name = { "hostname", "Half-Life" };

static cvar_t ipname = { "ip", "localhost" };
cvar_t iphostport = { "ip_hostport", "0" };
cvar_t hostport = { "hostport", "0" };
static cvar_t defport = { "port", PORT_SERVER };
static cvar_t ip_clientport = { "ip_clientport", "0" };
static cvar_t clientport = { "clientport", PORT_CLIENT };

#ifdef _WIN32
static cvar_t ipx_hostport = { "ipx_hostport", "0" };
static cvar_t ipx_clientport = { "ipx_clientport", "0" };
#endif

static cvar_t fakelag = { "fakelag", "0.0" };  // Lag all incoming network data (including loopback) by xxx ms.
static cvar_t fakeloss = { "fakeloss", "0.0" }; // Act like we dropped the packet this % of the time.

cvar_t noip = { "noip", "0" };    // Disable IP Support

netadr_t net_local_adr;

#ifdef _WIN32
cvar_t noipx = { "noipx", "0" };    // Disable IPX Support

netadr_t net_local_ipx_adr;
#endif

int			net_maxpacket = 0;
int			net_maxingame = 0;

netadr_t	net_from;

int			net_activeconnections = 0;

static byte net_message_buffer[MAX_MSGLEN];
sizebuf_t	net_message;

#ifdef _WIN32
WSADATA		winsockdata;
#endif

int			ip_sockets[2] = { 0, 0 };
#ifdef _WIN32
int			ipx_sockets[2] = { 0, 0 };
#endif

#define	MAX_LOOPBACK 4

typedef struct
{
	byte	data[MAX_MSGLEN + PACKET_HEADER + 1];
	int		datalen;
} loopmsg_t;

typedef struct
{
	loopmsg_t	msgs[MAX_LOOPBACK];
	int			get;
	int			send;
} loopback_t;

static loopback_t	loopbacks[2];

typedef struct packetlag_s
{
	char* pFileName; // Raw stream data is stored.
	int   nSize;
	netadr_t net_from;
	float receivedTime;
	struct packetlag_s* pNext;
	struct packetlag_s* pPrev;
} packetlag_t;

static packetlag_t g_pLagData[2];  // List of lag structures, if fakelag is set.

int losscount[2] = { 0, 0 };

char* NET_ErrorString( int code );

//=============================================================================

void NetadrToSockadr( netadr_t* a, struct sockaddr* s )
{
	memset(s, 0, sizeof(*s));

	if (a->type == NA_BROADCAST)
	{
		((struct sockaddr_in*)s)->sin_family = AF_INET;
		((struct sockaddr_in*)s)->sin_port = a->port;
		((struct sockaddr_in*)s)->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if (a->type == NA_IP)
	{
		((struct sockaddr_in*)s)->sin_family = AF_INET;
		((struct sockaddr_in*)s)->sin_addr.s_addr = *(int*)&a->ip;
		((struct sockaddr_in*)s)->sin_port = a->port;
	}
#ifdef _WIN32
	else if (a->type == NA_IPX)
	{
		((struct sockaddr_in*)s)->sin_family = AF_IPX;
		memcpy(&((struct sockaddr_in*)s)->sin_port, a->ipx, 4);
		memcpy(&((struct sockaddr_in*)s)->sin_addr.s_imp, &a->ipx[4], 6);
		*(unsigned short*)&((struct sockaddr_in*)s)->sin_zero[4] = a->port;
	}
	else if (a->type == NA_BROADCAST_IPX)
	{
		((struct sockaddr_in*)s)->sin_family = AF_IPX;
		memset(&((struct sockaddr_in*)s)->sin_port, 0, 4);
		memset(&((struct sockaddr_in*)s)->sin_addr.s_imp, 255, 6);
		*(unsigned short*)&((struct sockaddr_in*)s)->sin_zero[4] = a->port;
	}
#endif
}

void SockadrToNetadr( struct sockaddr* s, netadr_t* a )
{
	if (s->sa_family == AF_INET)
	{
		a->type = NA_IP;
		*(int*)&a->ip = ((struct sockaddr_in*)s)->sin_addr.s_addr;
		a->port = ((struct sockaddr_in*)s)->sin_port;
	}
#ifdef _WIN32
	else if (s->sa_family == AF_IPX)
	{
		a->type = NA_IPX;
		memcpy(a->ipx, &((struct sockaddr_in*)s)->sin_port, 4);
		memcpy(&a->ipx[4], &((struct sockaddr_in*)s)->sin_addr.s_imp, 6);
		a->port = *(unsigned short*)&((struct sockaddr_in*)s)->sin_zero[4];
	}
#endif // _WIN32
}

qboolean NET_CompareAdr( netadr_t a, netadr_t b )
{
	if (a.type != b.type)
		return FALSE;

	if (a.type == NA_LOOPBACK)
		return TRUE;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
			return TRUE;

		return FALSE;
	}
#ifdef _WIN32
	else if (a.type == NA_IPX)
	{
		if (memcmp(a.ipx, b.ipx, 10) == 0 && a.port == b.port)
			return TRUE;

		return FALSE;
	}
#endif

	return FALSE;
}

qboolean NET_CompareClassBAdr( netadr_t a, netadr_t b )
{
	if (a.type != b.type)
		return FALSE;

	if (a.type == NA_LOOPBACK)
		return TRUE;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
			return TRUE;

		return FALSE;
	}
#ifdef _WIN32
	else if (a.type == NA_IPX)
	{
		if (memcmp(a.ipx, b.ipx, 10) == 0)
			return TRUE;

		return FALSE;
	}
#endif

	return FALSE;
}

char* NET_AdrToString( netadr_t a )
{
	static char s[64];

	memset(s, 0, sizeof(s));

	if (a.type == NA_LOOPBACK)
		sprintf(s, "loopback");
	else if (a.type == NA_IP)
		sprintf(s, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
#ifdef _WIN32
	else
		sprintf(s, "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%i", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9], ntohs(a.port));
#endif
	
	return s;
}

char* NET_BaseAdrToString( netadr_t a )
{
	static char s[64];

	memset(s, 0, sizeof(s));

	if (a.type == NA_LOOPBACK)
		sprintf(s, "loopback");
	else if (a.type == NA_IP)
		sprintf(s, "%i.%i.%i.%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
#ifdef _WIN32
	else
		sprintf(s, "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9]);
#endif

	return s;
}

qboolean NET_StringToSockaddr( char* s, struct sockaddr* sadr )
{
	struct hostent* h;
	char* colon;
	int		val = 0;
	char	copy[128];

	memset(sadr, 0, sizeof(*sadr));

#ifdef _WIN32
	// IPX support
	if (strlen(s) >= 23 && s[8] == ':' && s[21] == ':')
	{
		int i;
		sadr->sa_family = AF_IPX;
		val = 0;
		for (i = 0; i < 20; i += 2)
		{
			// Convert from hexademical represantation to sockaddr
			char temp[3] = { s[i], s[i + 1], '\0' };
			sscanf(temp, "%x", &val);
			sadr->sa_data[i / 2] = (char)val;
		}

		sscanf(s + 22, "%u", &val);
		*(uint16*)&sadr->sa_data[10] = htons(val);

		return TRUE;
	}
#endif // _WIN32

	((struct sockaddr_in*)sadr)->sin_family = AF_INET;
	((struct sockaddr_in*)sadr)->sin_port = 0;

	strcpy(copy, s);

	// strip off a trailing :port if present
	for (colon = copy; *colon; colon++)
	{
		if (*colon == ':')
		{
			*colon = 0;
			((struct sockaddr_in*)sadr)->sin_port = htons(atoi(colon + 1));
		}
	}

	if (copy[0] < '0' || copy[0] > '9')
	{
		if (!(h = gethostbyname(copy)))
			return FALSE;
		*(int*)&((struct sockaddr_in*)sadr)->sin_addr = *(int*)h->h_addr_list[0];
	}
	else
	{
		*(int*)&((struct sockaddr_in*)sadr)->sin_addr = inet_addr(copy);
	}

	return TRUE;
}

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
qboolean NET_StringToAdr( char* s, netadr_t* a )
{
	struct sockaddr sadr;

	if (!strcmp(s, "localhost"))
	{
		memset(a, 0, sizeof(*a));
		a->type = NA_LOOPBACK;
		return TRUE;
	}

	if (!NET_StringToSockaddr(s, &sadr))
		return FALSE;

	SockadrToNetadr(&sadr, a);
	return TRUE;
}

qboolean NET_IsLocalAddress( netadr_t adr )
{
	return adr.type == NA_LOOPBACK;
}

/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/

qboolean NET_GetLoopPacket( netsrc_t sock, netadr_t* in_from, sizebuf_t* msg )
{
	int		i;
	loopback_t* loop;

	loop = &loopbacks[sock];

	if (loop->send - loop->get > MAX_LOOPBACK)
		loop->get = loop->send - MAX_LOOPBACK;

	if (loop->get >= loop->send)
	{
		return FALSE;
	}

	i = loop->get & (MAX_LOOPBACK - 1);
	loop->get++;

	memcpy(msg->data, &loop->msgs[i].data[0], loop->msgs[i].datalen);
	msg->cursize = loop->msgs[i].datalen;

	memset(in_from, 0, sizeof(*in_from));
	in_from->type = NA_LOOPBACK;

	return TRUE;
}

void NET_SendLoopPacket( netsrc_t sock, int length, void* data )
{
	int		i;
	loopback_t* loop;

	loop = &loopbacks[sock ^ 1];

	i = loop->send & (MAX_LOOPBACK - 1);
	loop->send++;

	memcpy(loop->msgs[i].data, data, length);
	loop->msgs[i].datalen = length;
}

//=============================================================================

/*
==================
NET_RemoveFromPacketList(packetlag_t *pPacket)

Unlinks it from the current list.
==================
*/
void NET_RemoveFromPacketList( packetlag_t* pPacket )
{
	pPacket->pPrev->pNext = pPacket->pNext;
	pPacket->pNext->pPrev = pPacket->pPrev;

	pPacket->pNext = pPacket->pPrev = NULL;
}

/*
==================
NET_ClearLaggedList(packetlag_t *pList)

==================
*/
void NET_ClearLaggedList( packetlag_t* pList )
{
	packetlag_t* p, * n;

	for (p = pList->pNext; p && p != pList;)
	{
		n = p->pNext;

		NET_RemoveFromPacketList(p);

		// Delete the associated file.
		_unlink(p->pFileName);

		if (p->pFileName)
			free(p->pFileName);
		p->pFileName = NULL;

		free(p);
		p = n;
	}

	pList->pNext = pList->pPrev = pList;
}

/*
===================
NET_AddToLagged

===================
*/
void NET_AddToLagged( netsrc_t sock, packetlag_t* pList, packetlag_t* pPacket, netadr_t* net_from, sizebuf_t messagedata )
{
	char szDumpFile[MAX_PATH];
	char* pStart;
	FILE* file;

	if (pPacket->pPrev || pPacket->pNext)
	{
		Con_Printf("Packet already linked\n");
		return;
	}

	pPacket->pPrev = pList->pPrev;
	pList->pPrev->pNext = pPacket;
	pList->pPrev = pPacket;
	pPacket->pNext = pList;

	sprintf(szDumpFile, "%s%i_%i.tmp", "c:\\temp\\N_", sock, losscount[sock]);

	losscount[sock]++;

	if (losscount[sock] > 8192)
		losscount[sock] = 0;

	pStart = (char*)malloc(strlen(szDumpFile) + 1);
	strcpy(pStart, szDumpFile);
	pPacket->pFileName = pStart;
	pPacket->nSize = messagedata.cursize;
	pPacket->receivedTime = realtime;   // Our time stamp.
	pPacket->net_from = *net_from;

	// Open the associated file.
	file = fopen(pStart, "wb");
	fwrite(messagedata.data, messagedata.cursize, 1, file);
	fclose(file);
}


qboolean NET_LagPacket( qboolean newdata, netsrc_t sock, netadr_t* from, sizebuf_t* data )
{
	packetlag_t* pNewPacketLag;
	packetlag_t* pPacket;
	FILE* file;

	if (fakelag.value <= 0.0)
	{
		// Never leave any old msgs around
		return newdata;
	}

	if (newdata)
	{
		if (fakeloss.value > 1.0f)
		{
			// Act like we didn't hear anything if we are going to lose the packet.
			// Depends on random # generator.
			if (RandomLong(0, 100) <= (int)fakeloss.value)
				return FALSE;
		}

		pNewPacketLag = (packetlag_t*)malloc(sizeof(packetlag_t));
		memset(pNewPacketLag, 0, sizeof(packetlag_t));

		NET_AddToLagged(sock, &g_pLagData[sock], pNewPacketLag, from, *data);
	}

	// Now check the correct list and feed any message that is old enought.
	pPacket = g_pLagData[sock].pNext;

	// Find an old enough packet.  If none are old enough, return false.
	while (pPacket != &g_pLagData[sock])
	{
		if (pPacket->receivedTime <= (realtime - (fakelag.value / 1000.0f)))
		{
			NET_RemoveFromPacketList(pPacket);

			file = fopen(pPacket->pFileName, "rb");
			fread(net_message.data, pPacket->nSize, 1, file);
			net_message.cursize = pPacket->nSize;
			net_from = pPacket->net_from;
			fclose(file);

			_unlink(pPacket->pFileName);
			free(pPacket->pFileName);
			free(pPacket);
			return TRUE;
		}

		pPacket = pPacket->pNext;
	}

	return FALSE;
}

//=============================================================================

qboolean NET_GetPacket( netsrc_t sock )
{
	int				ret;
	struct sockaddr	from;
	int				fromlen;
	int				net_socket = 0;
	int				protocol;
	int				err;

	// If we got a message from the loopback system, see if it should be lagged.
	if (NET_GetLoopPacket(sock, &net_from, &net_message))
	{
		return NET_LagPacket(TRUE, sock, &net_from, &net_message);
	}

	// No loopback, if not threaded, see if we got any over wire?
	for (protocol = 0; protocol < 2; protocol++)
	{
		if (protocol == 0)
			net_socket = ip_sockets[sock];
		else
			net_socket = ipx_sockets[sock];

		if (net_socket)
		{
			fromlen = sizeof(from);
			ret = recvfrom(net_socket, (char*)net_message.data, net_message.maxsize, 0, &from, &fromlen);
			if (ret != -1)
			{
				SockadrToNetadr(&from, &net_from);

				if (net_message.maxsize != ret)
				{
					// Transfer data
					net_message.cursize = ret;

					// Lag the packet, if needed
					return NET_LagPacket(TRUE, sock, &net_from, &net_message);
				}
				else
				{
					Con_Printf("Oversize packet from %s\n", NET_AdrToString(net_from));
				}
			}
			else
			{
#if defined( _WIN32 )
				err = WSAGetLastError();
#else
				err = errno;
#endif
				switch (err)
				{
				case WSAEWOULDBLOCK:
					break;

				case WSAEMSGSIZE:
					Con_DPrintf("Ignoring oversized network message\n");
					break;

				default:
					if (cls.state != ca_dedicated)
					{
						Sys_Error("NET_GetPacket: %s", NET_ErrorString(err));
						break;
					}

					// Let's continue even after errors
					Con_Printf("NET_GetPacket: %s", NET_ErrorString(err));
					break;
				}
			}
		}
	}

	// Allow lagging system to return a packet
	return NET_LagPacket(FALSE, sock, NULL, NULL);
}

//=============================================================================

void NET_SendPacket( netsrc_t sock, int length, void* data, netadr_t to )
{
	int		ret;
	struct sockaddr	addr;
	int		net_socket;

	if (length > net_maxpacket)
		net_maxpacket = length;

	if (cls.state == ca_active && length > net_maxingame)
		net_maxingame = length;

	if (to.type == NA_LOOPBACK)
	{
		NET_SendLoopPacket(sock, length, data);
		return;
	}

	if (to.type == NA_BROADCAST)
	{
		net_socket = ip_sockets[sock];
		if (!net_socket)
			return;
	}
	else if (to.type == NA_IP)
	{
		net_socket = ip_sockets[sock];
		if (!net_socket)
			return;
	}
#ifdef _WIN32
	else if (to.type == NA_IPX)
	{
		net_socket = ipx_sockets[sock];
		if (!net_socket)
			return;
	}
	else if (to.type == NA_BROADCAST_IPX)
	{
		net_socket = ipx_sockets[sock];
		if (!net_socket)
			return;
	}
#endif
	else
	{
		Sys_Error("NET_SendPacket: bad address type");
		return;
	}

	NetadrToSockadr(&to, &addr);

	ret = sendto(net_socket, (const char*)data, length, 0, &addr, sizeof(addr));
	if (ret == -1)
	{
		int err;

#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif
		// wouldblock is silent
		if (err == WSAEWOULDBLOCK)
			return;

		// some PPP links dont allow broadcasts
		if ((err == WSAEADDRNOTAVAIL) && (to.type == NA_BROADCAST
#if defined( _WIN32 )
			|| to.type == NA_BROADCAST_IPX
#endif
			))
			return;

		if (cls.state == ca_dedicated)	// let dedicated servers continue after errors
		{
			Con_Printf("NET_SendPacket ERROR: %s\n", NET_ErrorString(err));
		}
		else
		{
			if (err == WSAEADDRNOTAVAIL)
			{
				Con_DPrintf("NET_SendPacket Warning: %s : %s\n", NET_ErrorString(err), NET_AdrToString(to));
			}
			else
			{
				Sys_Error("NET_SendPacket ERROR: %s\n", NET_ErrorString(err));
			}
		}
	}
}

//=============================================================================

/*
====================
NET_IPSocket
====================
*/
int NET_IPSocket( char* net_interface, int port )
{
	int					newsocket;
	struct sockaddr_in	address;
	qboolean			_true = TRUE;
	int					i = 1;
	int					err;

	if ((newsocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif
		if (err != WSAEAFNOSUPPORT)
			Con_Printf("WARNING: UDP_OpenSocket: socket: %s", NET_ErrorString(err));

		return 0;
	}

	// make it non-blocking
	if (ioctlsocket(newsocket, FIONBIO, (unsigned long*)&_true) == -1)
	{
#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif	
		Con_Printf("WARNING: UDP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString(err));
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char*)&i, sizeof(i)) == -1)
	{
#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif	
		Con_Printf("WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString(err));
		return 0;
	}

	// make it reusable
	if (setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, (char*)&_true, sizeof(qboolean)) == -1)
	{
#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif
		Con_Printf("WARNING: UDP_OpenSocket: setsockopt SO_REUSEADDR: %s\n", NET_ErrorString(err));
		return 0;
	}

	if (!net_interface || !net_interface[0] || !_stricmp(net_interface, "localhost"))
		address.sin_addr.s_addr = INADDR_ANY;
	else
		NET_StringToSockaddr(net_interface, (struct sockaddr*)&address);

	if (port == PORT_ANY)
	{
		address.sin_port = 0;
	}
	else
	{
		address.sin_port = htons(port);
	}

	address.sin_family = AF_INET;

	if (bind(newsocket, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
#if defined( _WIN32 )
		err = WSAGetLastError();
#else
		err = errno;
#endif
		Con_Printf("WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString(err));
		closesocket(newsocket);
		return 0;
	}

	return newsocket;
}

/*
====================
NET_OpenIP

Opens the socket for IP communication
====================
*/
void NET_OpenIP( void )
{
	int		port;
	int		dedicated;

	dedicated = cls.state == ca_dedicated;

	if (!ip_sockets[NS_SERVER])
	{
		port = (int)iphostport.value;
		if (!port)
		{
			port = (int)hostport.value;
			if (!port)
			{
				port = (int)defport.value;
			}
		}

		ip_sockets[NS_SERVER] = NET_IPSocket(ipname.string, port);
		if (!ip_sockets[NS_SERVER] && dedicated)
		{
			Sys_Error("Couldn't allocate dedicated server IP port");
		}
	}

	// dedicated servers don't need client ports
	if (cls.state == ca_dedicated)
		return;

	if (!ip_sockets[NS_CLIENT])
	{
		port = (int)ip_clientport.value;
		if (!port)
		{
			port = (int)clientport.value;
			if (!port)
			{
				port = PORT_ANY;
			}
		}

		ip_sockets[NS_CLIENT] = NET_IPSocket(ipname.string, port);
		if (!ip_sockets[NS_CLIENT])
		{
			ip_sockets[NS_CLIENT] = NET_IPSocket(ipname.string, PORT_ANY);
		}
	}
}

#if defined( _WIN32 )
/*
====================
NET_IPXSocket
====================
*/
int NET_IPXSocket( int hostshort )
{
	int					newsocket;
	SOCKADDR_IPX		address;
	qboolean			_true = TRUE;
	int					err;
	
	if ((newsocket = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX)) == -1)
	{
		err = WSAGetLastError();
		if (err != WSAEAFNOSUPPORT)
		{
			Con_Printf("WARNING: IPX_Socket: socket: %s\n", NET_ErrorString(err));
		}
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket(newsocket, FIONBIO, (unsigned long*)&_true) == -1)
	{
		err = WSAGetLastError();
		Con_Printf("WARNING: IPX_Socket: ioctl FIONBIO: %s\n", NET_ErrorString(err));
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (const char*)&_true, sizeof(qboolean)) == -1)
	{
		err = WSAGetLastError();
		Con_Printf("WARNING: IPX_Socket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString(err));
		return 0;
	}

	address.sa_family = AF_IPX;
	memset(address.sa_netnum, 0, 4);
	memset(address.sa_nodenum, 0, 6);

	if (hostshort == PORT_ANY)
	{
		address.sa_socket = 0;
	}
	else
	{
		address.sa_socket = htons(hostshort);
	}

	if (bind(newsocket, (struct sockaddr*)&address, sizeof(SOCKADDR_IPX)) == -1)
	{
		err = WSAGetLastError();
		Con_Printf("WARNING: IPX_Socket: bind: %s\n", NET_ErrorString(err));
		closesocket(newsocket);
		return 0;
	}

	return newsocket;
}

/*
====================
NET_OpenIPX
====================
*/
void NET_OpenIPX( void )
{
	int		port;
	int		dedicated;

	dedicated = cls.state == ca_dedicated;

	if (!ipx_sockets[NS_SERVER])
	{
		port = (int)ipx_hostport.value;
		if (!port)
		{
			port = (int)hostport.value;
			if (!port)
			{
				port = (int)defport.value;
			}
		}

		ipx_sockets[NS_SERVER] = NET_IPXSocket(port);
	}

	// dedicated servers don't need client ports
	if (cls.state == ca_dedicated)
		return;

	if (!ipx_sockets[NS_CLIENT])
	{
		port = (int)ipx_clientport.value;
		if (!port)
		{
			port = (int)clientport.value;
			if (!port)
				port = PORT_ANY;
		}

		ipx_sockets[NS_CLIENT] = NET_IPXSocket(port);
		if (!ipx_sockets[NS_CLIENT])
		{
			ipx_sockets[NS_CLIENT] = NET_IPXSocket(PORT_ANY);
		}
	}
}

#endif

/*
====================
NET_Config

A single player game will only use the loopback code
====================
*/
void NET_Config( qboolean multiplayer )
{
	int		i;
	static	qboolean	old_config;

	if (old_config == multiplayer)
		return;
	
	old_config = multiplayer;
	if (!multiplayer)
	{
		// shut down any existing sockets
		for (i = 0; i < 2; i++)
		{
			if (ip_sockets[i])
			{
				closesocket(ip_sockets[i]);
				ip_sockets[i] = 0;
			}

#if defined( _WIN32 )
			if (ipx_sockets[i])
			{
				closesocket(ipx_sockets[i]);
				ipx_sockets[i] = 0;
			}
#endif
		}
	}
	else
	{
		// open sockets

		if (!noip.value)
		{
			NET_OpenIP();
		}

#if defined( _WIN32 )
		if (!noipx.value)
		{
			NET_OpenIPX();
		}
#endif
	}
}

/*
================
NET_GetLocalAddress

Returns the servers' ip address as a string.
================
*/
void NET_GetLocalAddress( void )
{
	char	buff[512];
	struct sockaddr_in	address;
	int		namelen;
	int     net_error = 0;

	memset(&net_local_adr, 0, sizeof(netadr_t));
#if defined( _WIN32 )
	memset(&net_local_ipx_adr, 0, sizeof(netadr_t));
#endif

	if (noip.value)
	{
		Con_Printf("TCP/IP Disabled.\n");
	}
	else
	{
		gethostname(buff, sizeof(buff));

		// Ensure that it doesn't overrun the buffer
		buff[sizeof(buff) - 1] = 0;

		NET_StringToAdr(buff, &net_local_adr);

		namelen = sizeof(address);
		if (getsockname(ip_sockets[NS_SERVER], (struct sockaddr*)&address, (socklen_t*)&namelen) != 0)
		{
			Cvar_SetValue("noip", 1.0f);
			net_error = errno;
			Con_Printf("Could not get TCP/IP address, TCP/IP disabled\nReason:  %s\n", NET_ErrorString(net_error));
		}
		else
		{
			net_local_adr.port = address.sin_port;
			Con_Printf("Server IP address %s\n", NET_AdrToString(net_local_adr));
		}
	}

#if defined( _WIN32 )
	if (noipx.value)
	{
		Con_Printf("No IPX Support.\n");
	}
	else
	{
		namelen = sizeof(SOCKADDR_IPX);
		if (getsockname(ipx_sockets[NS_SERVER], (struct sockaddr*)&address, &namelen) != 0)
		{
			Cvar_SetValue("noipx", 1.0f);
			net_error = errno;
			Con_Printf("Could not get IPX socket name, IPX disabled\nReason:  %s\n", NET_ErrorString(net_error));
		}
		else
		{
			SockadrToNetadr((struct sockaddr*)&address, &net_local_ipx_adr);
			Con_Printf("Server IPX address %s\n", NET_AdrToString(net_local_ipx_adr));
		}
	}
#endif
}

void NET_ShowMaxPacketSizes_f( void )
{
	Con_Printf("Max Packet     :  %i\n", net_maxpacket);
	Con_Printf("Max Game Packet:  %i\n", net_maxingame);
}

void Net_BadConnection_f( void )
{
	if (cls.state != ca_active)
		return;

	Cbuf_AddText(va("fakelag 200\nfakeloss 10\nnetchokeloop 1\nr_netgraph 1\n"));
	Con_Printf("Simulating a poor connection\n");
	Cbuf_AddText("ping\n");
}

/*
====================
MaxPlayers_f

How many players can connect to server
====================
*/
void MaxPlayers_f( void )
{
	int n;

	// We just want to know how many players can connect to this server
	if (Cmd_Argc() != 2)
	{
		Con_Printf("\"maxplayers\" is \"%u\"\n", svs.maxclients);
		return;
	}

	if (sv.active)
	{
		Con_Printf("maxplayers cannot be changed while a server is running.\n");
		return;
	}

	n = Q_atoi(Cmd_Argv(1));

	if (n < 1)
		n = 1;

	if (n > svs.maxclientslimit)
	{
		Con_Printf("\"maxplayers\" set to \"%u\"\n", svs.maxclientslimit);
		n = svs.maxclientslimit;
	}

	// Set maxclients
	svs.maxclients = n;

	if (n == 1)
		Cvar_Set("deathmatch", "0");
	else
		Cvar_Set("deathmatch", "1");
}

/*
====================
NET_Init

====================
*/
void NET_Init( void )
{
	int		i;
	int		r;
	int		hPort;

	r = WSAStartup(MAKEWORD(1, 1), &winsockdata);

	if (r)
		Sys_Error("Winsock initialization failed.");

	Cmd_AddCommand("maxplayers", MaxPlayers_f);
	Cmd_AddCommand("netbad", Net_BadConnection_f);

	Cvar_RegisterVariable(&noipx);
	Cvar_RegisterVariable(&noip);

	if (COM_CheckParm("-noipx"))
	{
		Cvar_SetValue("noipx", 1.0f);
	}

	if (COM_CheckParm("-noip"))
	{
		Cvar_SetValue("noip", 1.0f);
	}

	Cvar_RegisterVariable(&ipname);
	Cvar_RegisterVariable(&iphostport);
	Cvar_RegisterVariable(&hostport);
	Cvar_RegisterVariable(&defport);
	Cvar_RegisterVariable(&ip_clientport);
	Cvar_RegisterVariable(&clientport);
	Cvar_RegisterVariable(&ipx_hostport);
	Cvar_RegisterVariable(&ipx_clientport);

	// Parameters.

	hPort = COM_CheckParm("-port");
	if (hPort)
	{
		Cvar_SetValue("hostport", atof(com_argv[hPort + 1]));
	}

	Cmd_AddCommand("netmax", NET_ShowMaxPacketSizes_f);

	//
	// init the message buffer
	//
	net_message.maxsize = sizeof(net_message_buffer);
	net_message.data = net_message_buffer;

	for (i = 0; i < 2; i++)
	{
		g_pLagData[i].pNext = g_pLagData[i].pPrev = &g_pLagData[i];  // List of lag structures, if fakelag is set.
	}

	Cvar_RegisterVariable(&fakelag);
	Cvar_RegisterVariable(&fakeloss);
	Cvar_RegisterVariable(&host_name);

	NET_Config(TRUE);

	// Get our local address, if possible
	NET_GetLocalAddress();


	// TODO: Implement


	Con_Printf("Networking initialized.\n");
}

/*
====================
NET_Shutdown

====================
*/
void NET_Shutdown( void )
{
	NET_ClearLaggedList(&g_pLagData[0]);
	NET_ClearLaggedList(&g_pLagData[1]);

	NET_Config(FALSE);

	WSACleanup();
}




void NET_InitColors( void )
{
	// TODO: Implement
}


// TODO: Implement (Net graph code)



#if defined( _WIN32 )
/*
====================
NET_ErrorString
====================
*/
char* NET_ErrorString( int code )
{
	switch (code)
	{
	case WSAEINTR: return "WSAEINTR";
	case WSAEBADF: return "WSAEBADF";
	case WSAEACCES: return "WSAEACCES";
	case WSAEDISCON: return "WSAEDISCON";
	case WSAEFAULT: return "WSAEFAULT";
	case WSAEINVAL: return "WSAEINVAL";
	case WSAEMFILE: return "WSAEMFILE";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS: return "WSAEINPROGRESS";
	case WSAEALREADY: return "WSAEALREADY";
	case WSAENOTSOCK: return "WSAENOTSOCK";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE: return "WSAEMSGSIZE";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN: return "WSAENETDOWN";
	case WSAENETUNREACH: return "WSAENETUNREACH";
	case WSAENETRESET: return "WSAENETRESET";
	case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: return "WSAECONNRESET";
	case WSAENOBUFS: return "WSAENOBUFS";
	case WSAEISCONN: return "WSAEISCONN";
	case WSAENOTCONN: return "WSAENOTCONN";
	case WSAESHUTDOWN: return "WSAESHUTDOWN";
	case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
	case WSAETIMEDOUT: return "WSAETIMEDOUT";
	case WSAECONNREFUSED: return "WSAECONNREFUSED";
	case WSAELOOP: return "WSAELOOP";
	case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
	case WSASYSNOTREADY: return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: return "WSATRY_AGAIN";
	case WSANO_RECOVERY: return "WSANO_RECOVERY";
	case WSANO_DATA: return "WSANO_DATA";
	default: return "NO ERROR";
	}
}
#endif