// net.h -- Half-Life's interface to the networking layer
// For banning IP addresses (or allowing private games)
#ifndef NET_H
#define NET_H
#ifdef _WIN32
#pragma once
#endif


extern	sizebuf_t	net_message;



// Start up/shut down sockets layer
void		NET_Config( qboolean multiplayer );

#endif // NET_H