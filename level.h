// level.h
//

#ifndef LEVEL_H_31AE019508B493103BF90EFB5C5BDDE0
#define LEVEL_H_31AE019508B493103BF90EFB5C5BDDE0

#include "icon.h"

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

class ObjectType {
public:
    inline	ObjectType (int nx = 0, int ny = 0, PicIndex npic = FloorPix) : x (nx), y (ny), pic (npic) {}
    void	read (istream& is);
    void	write (ostream& os) const;
    size_t	stream_size (void) const;
public:
    int 	x;
    int 	y;
    PicIndex	pic;
};

class Level {
public:
			Level (void);
    void		Draw (CGC& gc) const;
    void		Draw (CGC& gc, coord_t x, coord_t y) const;
    void		MoveRobot (RobotDir where);
    void		MoveRobot (coord_t x, coord_t y, PicIndex pic);
    void		DisposeCrate (uoff_t index);
    void		AddCrate (coord_t x, coord_t y, PicIndex pic);
    void		SetCell (coord_t x, coord_t y, PicIndex pic);
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
    bool		Finished (void) const;
    int			FindCrate (coord_t x, coord_t y) const;
    bool		CanMoveTo (coord_t x, coord_t y, RobotDir where) const;
    inline PicIndex	At (coord_t x, coord_t y) const	{ return (m_Map [y * MAP_WIDTH + x]); }
private:
    vector<PicIndex>	m_Map;
    vector<ObjectType>	m_Crates;
    ObjectType		m_Robot;
    size_t		m_nCrates;
};

STD_STREAMABLE (ObjectType)
STD_STREAMABLE (Level)
CAST_STREAMABLE (PicIndex, int)

#endif

