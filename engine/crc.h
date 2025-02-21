/* crc.h */
#ifndef CRC_H
#define CRC_H
#ifdef _WIN32
#pragma once
#endif

typedef unsigned long CRC32_t;

void CRC32_Init( CRC32_t* pulCRC );
CRC32_t CRC32_Final( CRC32_t pulCRC );
void CRC32_ProcessBuffer( CRC32_t* pulCRC, void* p, int len );
void CRC32_ProcessByte( CRC32_t* pulCRC, unsigned char ch );
int CRC_File( CRC32_t* crcvalue, char* pszFileName );
byte COM_BlockSequenceCRCByte( byte* base, int length, int sequence );

void MD5Init( MD5Context_t* context );
void MD5Update( MD5Context_t* context, unsigned char const* buf,
    unsigned int len );
void MD5Final( unsigned char* digest, MD5Context_t* context );
void Transform( unsigned int buf[4], unsigned int const in[16] );

int MD5_Hash_File( unsigned char* digest, char* pszFileName );
char* MD5_Print( unsigned char* hash );
int MD5_Hash_CachedFile( unsigned char digest[16], unsigned char* pCache, int nFileSize, int bSeed, unsigned int seed[4] );

int CRC_MapFile( CRC32_t* crcvalue, char* pszFileName );

#endif