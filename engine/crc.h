/* crc.h */
#ifndef CRC_H
#define CRC_H
#ifdef _WIN32
#pragma once
#endif


#ifdef _WIN32
typedef unsigned long CRC32_t;
#else
typedef unsigned long CRC32_t;
#endif

void CRC32_Init( CRC32_t* pulCRC );
CRC32_t CRC32_Final( CRC32_t pulCRC );
void CRC32_ProcessBuffer( CRC32_t* pulCRC, void* p, int len );
void CRC32_ProcessByte( CRC32_t* pulCRC, unsigned char ch );
int CRC_File( CRC32_t* crcvalue, char* pszFileName );


#endif