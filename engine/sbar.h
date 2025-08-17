#ifndef SBAR_H
#define SBAR_H
#pragma once

void HudSizeUp( void );
void HudSizeDown( void );

void Sbar_Geiger( void );

// the status bar is only redrawn if something has changed, but if anything
// does, the entire thing will be redrawn for the next vid.numpages frames.
void Sbar_Draw( void );

void FillBackGround( void );

#endif // SBAR_H