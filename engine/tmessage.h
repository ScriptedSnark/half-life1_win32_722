#ifndef TMESSAGE_H
#define TMESSAGE_H
#pragma once

// text message system
void					TextMessageInit( void );
client_textmessage_t*	TextMessageGet( const char* pName );

int TextMessageDrawCharacter( int x, int y, int number, int r, int g, int b );

#endif		//TMESSAGE_H