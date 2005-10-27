// This file is part of the GJID game.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// level.cc
//
//	Implements the level class.
//

#include "level.h"

//----------------------------------------------------------------------

void ObjectType::read (istream& is)
{
    is >> x >> y >> pic;
}

void ObjectType::write (ostream& os) const
{
    os << x << y << pic;
}

size_t ObjectType::stream_size (void) const
{
    return (stream_size_of(x) + stream_size_of(y) + stream_size_of(pic));
}

//----------------------------------------------------------------------

Level::Level (void)
: m_Map (MAP_WIDTH * MAP_HEIGHT),
  m_Objects ()
{
    m_Objects.push_back (ObjectType (0, 0, RobotNorthPix));
    fill (m_Map, tilemap_t::value_type(FloorPix));
}

void Level::Draw (CGC& gc, const picvec_t& tiles) const
{
    tilemap_t::const_iterator it (m_Map.begin());
    for (coord_t y = 0; y < MAP_HEIGHT; ++ y)
	for (coord_t x = 0; x < MAP_WIDTH; ++ x)
	    tiles [*it++].Put (gc, x * TILE_W, y * TILE_H);
    foreach (objvec_t::const_iterator, i, m_Objects)
	tiles[i->pic].PutMasked (gc, i->x * TILE_W, i->y * TILE_H);
}

bool Level::CanMoveTo (coord_t x, coord_t y, RobotDir where) const
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

int Level::FindCrate (coord_t x, coord_t y) const
{
    for (uoff_t i = 1; i < m_Objects.size(); ++ i)
	if (m_Objects[i].x == x && m_Objects[i].y == y)
	    return (i);
    return (-1);
}

void Level::MoveRobot (RobotDir where)
{
    int dx = 0, dy = 0;
    switch (where) {
	case North:	dy = -1;
			Robot().pic = RobotNorthPix;
			break;
	case South:	dy = 1;
			Robot().pic = RobotSouthPix;
			break;
	case East:	dx = 1;
			Robot().pic = RobotEastPix;
			break;
	case West:	dx = -1;
			Robot().pic = RobotWestPix;
			break;
    }

    // Check if map square can be moved on
    if (!CanMoveTo (Robot().x + dx, Robot().y + dy, where))
	return;

    // Check if a crate needs to be moved
    int ciw = FindCrate (Robot().x + dx, Robot().y + dy);
    if (ciw >= 0) {
	// Can only move one crate - this checks for another one behind ciw
	//	also checks if the square behind crate can be moved into
	if (FindCrate (Robot().x + 2 * dx, Robot().y + 2 * dy) < 0 && CanMoveTo (Robot().x + 2 * dx, Robot().y + 2 * dy, where)) {
	    m_Objects[ciw].x += dx;
	    m_Objects[ciw].y += dy;
	    Robot().x += dx;
	    Robot().y += dy;
	    if (At(m_Objects[ciw].x, m_Objects[ciw].y) == DisposePix)
		DisposeCrate (ciw);
	}
    } else {
	Robot().x += dx;
	Robot().y += dy;
    }
}

void Level::MoveRobot (coord_t x, coord_t y, PicIndex pic)
{
    if (CanMoveTo (x, y, North)) {
	Robot().x = x;
	Robot().y = y;
	Robot().pic = pic;
    }
}

void Level::AddCrate (coord_t x, coord_t y, PicIndex pic)
{
    m_Objects.push_back();
    m_Objects.back().x = x;
    m_Objects.back().y = y;
    m_Objects.back().pic = pic;
}

bool Level::Finished (void) const
{
    return (m_Objects.size() == 1 && At(Robot().x, Robot().y) == ExitPix);
}

void Level::read (istream& is)
{
    is >> m_Map;
    is >> m_Objects;
    if (m_Objects.empty())
	m_Objects.push_back (ObjectType (0, 0, RobotNorthPix));
}

void Level::write (ostream& os) const
{
    os << m_Map;
    os << m_Objects;
}

size_t Level::stream_size (void) const
{
    size_t s (0);
    s += stream_size_of (m_Map);
    s += stream_size_of (m_Objects);
    return (s);
}

