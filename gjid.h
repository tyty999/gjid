/* gjid.h
**
**	Defines puzzle game structures.
*/

#ifndef __GJID_H
#define __GJID_H

#include <mdefs.h>
#include <llist.h>
#include <streamable.h>
#include <game/mob.h>
#include <game/palette.h>

#define DATAFILE	LIBDIR##"/gjid.dat"
#define SQUARE_SIDE	16
#define MAP_WIDTH	20
#define MAP_HEIGHT	12

#define MAX_LEVELS	1000

typedef enum {
    DisposePix,
    ExitPix,
    FloorPix,
    OWDNorthPix,	// One-way door
    OWDSouthPix,
    OWDEastPix,
    OWDWestPix,
    Wall1Pix,
    Wall2Pix,
    Wall3Pix,
    Back1Pix,
    Back2Pix,
    Back3Pix,
    RobotNorthPix,
    RobotSouthPix,
    RobotEastPix,
    RobotWestPix,
    Barrel1Pix,
    Barrel2Pix,
    NumberOfMapPics,
    LogoGPix = NumberOfMapPics,
    LogoJPix,
    LogoIPix,
    LogoDPix,
    NumberOfPics
} PicIndex;

typedef enum {
    North,
    South,
    East,
    West
} RobotDir;

typedef struct {
    int 	x;
    int 	y;
    PicIndex	pic;
} ObjectType;

class Level : public Streamable {
private:
    PicIndex		Map [MAP_WIDTH] [MAP_HEIGHT];
    WORD		nCrates;
    ObjectType		Robot;
    LList<ObjectType>	Crates;

public:
			Level (void);
    void		Draw (void);
    void		Draw (WORD x, WORD y);
    void		MoveRobot (RobotDir where);
    void		MoveRobot (WORD x, WORD y, PicIndex pic);
    void		DisposeCrate (WORD index);
    void		AddCrate (WORD x, WORD y, PicIndex pic);
    void		SetCell (WORD x, WORD y, PicIndex pic);
    void		Read (ifstream& is);
    void		Write (ofstream& os);
    BOOL		Finished (void);
    int			FindCrate (WORD x, WORD y);
    BOOL		CanMoveTo (WORD x, WORD y, RobotDir where);
    Level&		operator= (Level& ToBe);
    virtual	       ~Level (void);
};

/*---------------------------------------------*/
 void 	LoadData (char * filename);
 void 	SaveData (char * filename);
 void	EditData (void);
 void 	PlayGame (void);
/*---------------------------------------------*/

extern Icon pics[];
extern PaletteType pal;
extern LList<Level> levels;
extern WORD nLevels;

#endif

