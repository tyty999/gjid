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

bool Level::CanMoveTo (uint8_t x, uint8_t y, RobotDir where) const
{
    if (x >= MAP_WIDTH || y >= MAP_HEIGHT)
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

int Level::FindCrate (uint8_t x, uint8_t y) const
{
    for (unsigned i = 0; i < _objects.size(); ++ i)
	if (_objects[i].x == x && _objects[i].y == y)
	    return (i);
    return (-1);
}

bool Level::MoveRobot (RobotDir where)
{
    static const struct {
	int8_t dx,dy;
	uint16_t img;
    } robotdir[] = {{0,-1,RobotNorthPix},{0,1,RobotSouthPix},{1,0,RobotEastPix},{-1,0,RobotWestPix}};

    _robot.pic = robotdir[where].img;
    uint8_t newx = _robot.x + robotdir[where].dx;
    uint8_t newcratex = newx + robotdir[where].dx;
    uint8_t newy = _robot.y + robotdir[where].dy;
    uint8_t newcratey = newy + robotdir[where].dy;

    // Check if map square can be moved on
    if (!CanMoveTo (newx, newy, where))
	return (false);

    // Check if a crate needs to be moved
    int ciw = FindCrate (newx, newy);
    if (ciw >= 0) {
	// Can only move one crate - this checks for another one behind ciw
	//	also checks if the square behind crate can be moved into
	if (FindCrate(newcratex, newcratey) >= 0 || !CanMoveTo(newcratex, newcratey, where))
	    return (false);
	_objects[ciw].x = newcratex;
	_objects[ciw].y = newcratey;
	if (At(newcratex, newcratey) == DisposePix)
	    DisposeCrate (ciw);
    }
    _robot.x = newx;
    _robot.y = newy;
    return (true);
}

const char* Level::Load (const char* ldata)
{
    static const char picToChar[NumberOfMapPics+1] = "0E.^v><#%+~!`   @NP";
    _objects.clear();
    for (uint8_t y = 0; y < MAP_HEIGHT; ++y) {
	for (uint8_t x = 0; x < MAP_WIDTH; ++x) {
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
