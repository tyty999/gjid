// gjid.h
//
//	Defines puzzle game structures.
//

#ifndef GJID_H_70CAC3810A6C8B6422EDC4165A5EA40C
#define GJID_H_70CAC3810A6C8B6422EDC4165A5EA40C

#include <palette.h>
#include "level.h"

#define DATAFILE	"/usr/games/lib/gjid.dat"

//----------------------------------------------------------------------

void 	LoadData (const char* filename);
void 	SaveData (const char* filename);
void	EditData (void);
void 	PlayGame (void);

//----------------------------------------------------------------------

extern Icon pics[];
extern PaletteType pal;
extern vector<Level> levels;
extern uint16_t nLevels;
extern string story;
extern uint16_t StorySize;

#endif

