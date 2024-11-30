// net.h -- Half-Life's interface to the networking layer
// For banning IP addresses (or allowing private games)
#ifndef NET_H
#define NET_H
#ifdef _WIN32
#pragma once
#endif


extern	sizebuf_t	net_message;


// Start up networking
void		NET_Init( void );



// Start up/shut down sockets layer
void		NET_Config( qboolean multiplayer );






// Initialize subsystem
void	Netchan_Init( void );



#endif // NET_H