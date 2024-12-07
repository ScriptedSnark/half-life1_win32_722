#ifndef CMODEL_H
#define CMODEL_H
#ifdef _WIN32
#pragma once
#endif

extern byte* gPAS;
extern byte* gPVS;

extern int gPVSRowBytes;
extern byte mod_novis[MAX_MAP_LEAFS / 8];

void	Mod_Init( void );
byte*	Mod_DecompressVis( byte* in, model_t* model );
byte*	Mod_LeafPVS( mleaf_t* leaf, model_t* model );

void	CM_DecompressPVS( byte* in, byte* decompressed, int byteCount );

byte*	CM_LeafPVS( int leafnum );
byte*	CM_LeafPAS( int leafnum );
void	CM_FreePAS( void );
void	CM_CalcPAS( model_t* pModel );

qboolean CM_HeadnodeVisible( mnode_t* node, byte* visbits );

#endif // CMODEL_H