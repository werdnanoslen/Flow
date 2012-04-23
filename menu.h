
//{{BLOCK(menu)

//======================================================================
//
//	menu, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 76 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 4864 + 2048 = 7424
//
//	Time-stamp: 2012-04-23, 00:59:47
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MENU_H
#define GRIT_MENU_H

#define menuTilesLen 4864
extern const unsigned short menuTiles[2432];

#define menuMapLen 2048
extern const unsigned short menuMap[1024];

#define menuPalLen 512
extern const unsigned short menuPal[256];

#endif // GRIT_MENU_H

//}}BLOCK(menu)
