#ifndef TMESSAGE_H
#define TMESSAGE_H
#pragma once

#define DEMO_MESSAGE "__DEMOMESSAGE__"

extern client_textmessage_t* gMessageTable;
extern int					gMessageTableCount;

char* memfgets( byte* pMemFile, int fileSize, int* pFilePos, char* pBuffer, int bufferSize );

// text message system
void					TextMessageInit( void );
client_textmessage_t*	TextMessageGet( const char* pName );

void SetDemoMessage( const char* pszMessage, float fFadeInTime, float fFadeOutTime, float fHoldTime );
int TextMessageDrawCharacter( int x, int y, int number, int r, int g, int b );

void TrimSpace( const char* source, char* dest );

#endif		//TMESSAGE_H