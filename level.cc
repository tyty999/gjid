// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "level.h"

//----------------------------------------------------------------------

Level::Level (void)
:_map (MAP_WIDTH * MAP_HEIGHT)
,_objects()
{
    fill (_map.begin(), _map.end(), tilemap_t::value_type(FloorPix));
    MoveRobot (0, 0, RobotNorthPix);
}

bool Level::CanMoveTo (int x, int y, RobotDir where) const
{
    if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
	return (false);
    const PicIndex tpic (At(x,y));
    if (tpic == DisposePix || tpic == ExitPix || tpic == FloorPix)
	return (true);
    else if (tpic == OWDNorthPix)	return (where == North);
    else if (tpic == OWDSouthPix)	return (where == South);
    else if (tpic == OWDEastPix)	return (where == East);
    else if (tpic == OWDWestPix)	return (where == West);
    return (false);
}

int Level::FindCrate (int x, int y) const
{
    for (uint32_t i = 0; i < _objects.size(); ++ i)
	if (_objects[i].x == x && _objects[i].y == y)
	    return (i);
    return (-1);
}

void Level::MoveRobot (RobotDir where)
{
    static const int8_t dirStepX[4] = { 0, 0, 1, -1 };
    static const int8_t dirStepY[4] = { -1, 1, 0, 0 };
    static const uint8_t dirImage[4] = { RobotNorthPix, RobotSouthPix, RobotEastPix, RobotWestPix };

    int dx = dirStepX [where], dy = dirStepY [where];
    _robot.pic = dirImage [where];

    // Check if map square can be moved on
    if (!CanMoveTo (_robot.x+dx, _robot.y+dy, where))
	return;

    // Check if a crate needs to be moved
    int ciw = FindCrate (_robot.x+dx, _robot.y+dy);
    if (ciw >= 0) {
	// Can only move one crate - this checks for another one behind ciw
	//	also checks if the square behind crate can be moved into
	if (FindCrate(_robot.x+2*dx, _robot.y+2*dy) < 0 && CanMoveTo(_robot.x+2*dx, _robot.y+2*dy, where)) {
	    _objects[ciw].x += dx;
	    _objects[ciw].y += dy;
	    _robot.x += dx;
	    _robot.y += dy;
	    if (At(_objects[ciw].x, _objects[ciw].y) == DisposePix)
		DisposeCrate (ciw);
	}
    } else {
	_robot.x += dx;
	_robot.y += dy;
    }
}

const char* Level::Load (const char* ldata)
{
    static const char picToChar[NumberOfMapPics+1] = "0E.^v><#%+~!`   @NP";
    _objects.clear();
    for (int y = 0; y < MAP_HEIGHT; ++y) {
	for (int x = 0; x < MAP_WIDTH; ++x) {
	    char c = *ldata++;
	    const char* pf = strchr (picToChar, c);
	    PicIndex pic = pf ? PicIndex(distance(picToChar,pf)) : FloorPix;
	    if (pic >= RobotNorthPix) {
		if (pic >= Barrel1Pix)
		    AddCrate (x, y, pic);
		else
		    MoveRobot (x, y, pic);
	    	pic = FloorPix;
	    }
	    _map[y*MAP_WIDTH+x] = pic;
	}
    }
    return (*ldata ? ldata : NULL);
}
