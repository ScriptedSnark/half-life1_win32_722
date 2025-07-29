// clientid.h
#ifndef CLIENTID_H
#define CLIENTID_H

typedef struct clientid_s
{
	int size;
	unsigned char* hash;
} clientid_t;

#endif