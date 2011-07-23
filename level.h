// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "icon.h"

//----------------------------------------------------------------------

#define TILE_W		16
#define TILE_H		16
#define MAP_WIDTH	20
#define MAP_HEIGHT	12

#define MAX_LEVELS	1000

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

typedef vector<Icon>	picvec_t;

//----------------------------------------------------------------------

class ObjectType {
public:
    inline	ObjectType (coord_t nx = 0, coord_t ny = 0, PicIndex npic = FloorPix) : x (nx), y (ny), pic (npic) {}
    void	read (istream& is);
    void	write (ostream& os) const;
    size_t	stream_size (void) const;
public:
    uint8_t 	x;
    uint8_t 	y;
    uint8_t	pic;
};

class Level {
public:
			Level (void);
    void		Draw (CGC& gc, const picvec_t& tiles) const;
    inline PicIndex	At (coord_t x, coord_t y) const			{ return (PicIndex (m_Map [y * MAP_WIDTH + x])); }
    inline void		SetCell (coord_t x, coord_t y, PicIndex pic)	{ m_Map [y * MAP_WIDTH + x] = pic; }
    bool		Finished (void) const;
    inline void		DisposeCrate (uoff_t index)			{ m_Objects.erase (m_Objects.begin() + index); }
    void		AddCrate (coord_t x, coord_t y, PicIndex pic);
    void		MoveRobot (RobotDir where);
    void		MoveRobot (coord_t x, coord_t y, PicIndex pic);
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
    int			FindCrate (coord_t x, coord_t y) const;
    bool		CanMoveTo (coord_t x, coord_t y, RobotDir where) const;
private:
    typedef vector<uint8_t>	tilemap_t;
    typedef vector<ObjectType>	objvec_t;
private:
    inline const ObjectType&	Robot (void) const	{ return (m_Objects[0]); }
    inline ObjectType&		Robot (void)		{ return (m_Objects[0]); }
private:
    tilemap_t		m_Map;
    objvec_t		m_Objects;
};

ALIGNOF (ObjectType, 1)
ALIGNOF (Level, 4)
STD_STREAMABLE (ObjectType)
STD_STREAMABLE (Level)
CAST_STREAMABLE (PicIndex, int)
