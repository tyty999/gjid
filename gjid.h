// gjid.h
//
//	Defines puzzle game structures.
//

#ifndef GJID_H_70CAC3810A6C8B6422EDC4165A5EA40C
#define GJID_H_70CAC3810A6C8B6422EDC4165A5EA40C

#include "level.h"

#define DATAFILE	"data/gjid.dat"

//----------------------------------------------------------------------

void 	LoadData (const char* filename);
void 	SaveData (const char* filename);
void	EditData (void);
void 	PlayGame (void);

//----------------------------------------------------------------------

typedef tuple<NumberOfPics,Icon>	picvec_t;
extern picvec_t pics;
extern CPalette pal;
extern vector<Level> levels;
extern size_t nLevels;
extern string story;
extern size_t StorySize;

#endif

