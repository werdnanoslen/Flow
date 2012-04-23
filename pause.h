
//{{BLOCK(pause)

//======================================================================
//
//	pause, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 71 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 4544 + 2048 = 7104
//
//	Time-stamp: 2012-04-22, 22:17:22
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_PAUSE_H
#define GRIT_PAUSE_H

#define pauseTilesLen 4544
extern const unsigned short pauseTiles[2272];

#define pauseMapLen 2048
extern const unsigned short pauseMap[1024];

#define pausePalLen 512
extern const unsigned short pausePal[256];

#endif // GRIT_PAUSE_H

//}}BLOCK(pause)
