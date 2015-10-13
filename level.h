// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

//----------------------------------------------------------------------

enum {
    TILE_W	= 16,
    TILE_H	= 16,
    MAP_WIDTH	= 20,
    MAP_HEIGHT	= 12
};

//----------------------------------------------------------------------

enum PicIndex {
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
};

enum RobotDir {
    North,
    South,
    East,
    West
};

//----------------------------------------------------------------------

class Level {
public:
    struct Object {
	uint8_t 	x;
	uint8_t 	y;
	uint16_t	pic;
	inline		Object (uint8_t nx = 0, uint8_t ny = 0, PicIndex npic = FloorPix) : x (nx), y (ny), pic (npic) {}
    };
    using tilemap_t	= vector<uint8_t>;
    using objvec_t	= vector<Object>;
    using rctilemap_t	= const tilemap_t&;
    using rcobjvec_t	= const objvec_t&;
public:
			Level (void);
    inline PicIndex	At (uint8_t x, uint8_t y) const		{ return PicIndex (_map[y*MAP_WIDTH+x]); }
    inline rctilemap_t	Map (void) const			{ return _map; }
    inline rcobjvec_t	Objects (void) const			{ return _objects; }
    const Object&	Robot (void) const			{ return _robot; }
    inline void		SetCell (uint8_t x, uint8_t y, PicIndex pic)	{ _map[y*MAP_WIDTH+x] = pic; }
    bool		Finished (void) const			{ return _objects.empty() && At(_robot.x, _robot.y) == ExitPix; }
    bool		MoveRobot (RobotDir where);
    const char*		Load (const char* ldata);
private:
    bool		CanMoveTo (uint8_t x, uint8_t y, RobotDir where) const noexcept;
    inline void		MoveRobot (uint8_t x, uint8_t y, PicIndex pic)	{ _robot.x = x; _robot.y = y; _robot.pic = pic; }
    int			FindCrate (uint8_t x, uint8_t y) const noexcept;
    inline void		AddCrate (uint8_t x, uint8_t y, PicIndex pic)	{ _objects.emplace_back (x, y, pic); }
    inline void		DisposeCrate (unsigned index)			{ _objects.erase (_objects.begin() + index); }
private:
    tilemap_t		_map;
    objvec_t		_objects;
    Object		_robot;
};
