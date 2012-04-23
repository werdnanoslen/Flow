
//{{BLOCK(startbg)

//======================================================================
//
//	startbg, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 601 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 38464 + 2048 = 41024
//
//	Time-stamp: 2012-04-22, 18:23:30
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STARTBG_H
#define GRIT_STARTBG_H

#define startbgTilesLen 38464
extern const unsigned short startbgTiles[19232];

#define startbgMapLen 2048
extern const unsigned short startbgMap[1024];

#define startbgPalLen 512
extern const unsigned short startbgPal[256];

#endif // GRIT_STARTBG_H

//}}BLOCK(startbg)
