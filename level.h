// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"

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

typedef vector<CImage>	picvec_t;

//----------------------------------------------------------------------

class Level {
public:
    class ObjectType {
    public:
	inline		ObjectType (coord_t nx = 0, coord_t ny = 0, PicIndex npic = FloorPix) : x (nx), y (ny), pic (npic) {}
	inline void	read (istream& s)	{ s >> x >> y >> pic; }
	inline void	write (ostream& s) const{ s << x << y << pic; }
	inline size_t	stream_size (void) const{ return (3); }
    public:
	uint8_t 	x;
	uint8_t 	y;
	uint8_t		pic;
    };
    typedef vector<uint8_t>	tilemap_t;
    typedef vector<ObjectType>	objvec_t;
    typedef const tilemap_t&	rctilemap_t;
    typedef const objvec_t&	rcobjvec_t;
public:
			Level (void);
    inline PicIndex	At (coord_t x, coord_t y) const			{ return (PicIndex (_map [y * MAP_WIDTH + x])); }
    inline void		SetCell (coord_t x, coord_t y, PicIndex pic)	{ _map [y * MAP_WIDTH + x] = pic; }
    bool		Finished (void) const;
    inline void		DisposeCrate (uoff_t index)			{ _objects.erase (_objects.begin() + index); }
    inline rctilemap_t	Map (void) const				{ return (_map); }
    inline rcobjvec_t	Objects (void) const				{ return (_objects); }
    void		AddCrate (coord_t x, coord_t y, PicIndex pic);
    void		MoveRobot (RobotDir where);
    void		MoveRobot (coord_t x, coord_t y, PicIndex pic);
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
    int			FindCrate (coord_t x, coord_t y) const;
    bool		CanMoveTo (coord_t x, coord_t y, RobotDir where) const;
private:
    inline const ObjectType&	Robot (void) const	{ return (_objects[0]); }
    inline ObjectType&		Robot (void)		{ return (_objects[0]); }
private:
    tilemap_t		_map;
    objvec_t		_objects;
};

ALIGNOF (Level, 4)
ALIGNOF (Level::ObjectType, 1)
CAST_STREAMABLE (PicIndex, int)
