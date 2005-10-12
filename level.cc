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
  m_Crates (),
  m_Robot (0, 0, RobotNorthPix),
  m_nCrates (0)
{
    fill (m_Map, FloorPix);
}

void Level::Draw (CGC& gc, const picvec_t& tiles) const
{
    tilemap_t::const_iterator it (m_Map.begin());
    for (coord_t y = 0; y < MAP_HEIGHT; ++ y)
	for (coord_t x = 0; x < MAP_WIDTH; ++ x)
	    tiles [*it++].Put (gc, x * SQUARE_SIDE, y * SQUARE_SIDE);
    foreach (objvec_t::const_iterator, i, m_Crates)
	tiles[i->pic].PutMasked (gc, i->x * SQUARE_SIDE, i->y * SQUARE_SIDE);
    tiles[m_Robot.pic].PutMasked (gc, m_Robot.x * SQUARE_SIDE, m_Robot.y * SQUARE_SIDE);
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
    foreach (objvec_t::const_iterator, ic, m_Crates)
	if (ic->x == x && ic->y == y)
	    return (distance (m_Crates.begin(), ic));
    return (-1);
}

void Level::SetCell (coord_t x, coord_t y, PicIndex pic)
{
    m_Map [y * MAP_WIDTH + x] = pic;
}

bool Level::Finished (void) const
{
    return (!m_nCrates && At(m_Robot.x, m_Robot.y) == ExitPix);
}

void Level::MoveRobot (RobotDir where)
{
    int dx = 0, dy = 0;
    switch (where) {
	case North:	dy = -1;
			m_Robot.pic = RobotNorthPix;
			break;
	case South:	dy = 1;
			m_Robot.pic = RobotSouthPix;
			break;
	case East:	dx = 1;
			m_Robot.pic = RobotEastPix;
			break;
	case West:	dx = -1;
			m_Robot.pic = RobotWestPix;
			break;
    }

    // Check if map square can be moved on
    if (!CanMoveTo (m_Robot.x + dx, m_Robot.y + dy, where))
	return;

    // Check if a crate needs to be moved
    int ciw = FindCrate (m_Robot.x + dx, m_Robot.y + dy);
    if (ciw >= 0) {
	// Can only move one crate - this checks for another one behind ciw
	//	also checks if the square behind crate can be moved into
	if (FindCrate (m_Robot.x + 2 * dx, m_Robot.y + 2 * dy) < 0 && CanMoveTo (m_Robot.x + 2 * dx, m_Robot.y + 2 * dy, where)) {
	    m_Crates[ciw].x += dx;
	    m_Crates[ciw].y += dy;
	    m_Robot.x += dx;
	    m_Robot.y += dy;
	    if (At(m_Crates[ciw].x, m_Crates[ciw].y) == DisposePix)
		DisposeCrate (ciw);
	}
    } else {
	m_Robot.x += dx;
	m_Robot.y += dy;
    }
}

void Level::MoveRobot (coord_t x, coord_t y, PicIndex pic)
{
    if (CanMoveTo (x, y, North)) {
	m_Robot.x = x;
	m_Robot.y = y;
	m_Robot.pic = pic;
    }
}

void Level::DisposeCrate (uoff_t index)
{
    m_Crates.erase (m_Crates.begin() + index);
    -- m_nCrates;
}

void Level::AddCrate (coord_t x, coord_t y, PicIndex pic)
{
    m_Crates.push_back();
    m_Crates.back().x = x;
    m_Crates.back().y = y;
    m_Crates.back().pic = pic;
    ++ m_nCrates;
}

void Level::read (istream& is)
{
    foreach (tilemap_t::iterator, i, m_Map)
	is >> *i;
    is >> m_nCrates;
    m_Crates.resize (m_nCrates);
    foreach (objvec_t::iterator, i, m_Crates)
	is >> *i;
    is >> m_Robot;
}

void Level::write (ostream& os) const
{
    foreach (tilemap_t::const_iterator, i, m_Map)
	os << *i;
    os << m_nCrates;
    foreach (objvec_t::const_iterator, i, m_Crates)
	os << *i;
    os << m_Robot;
}

size_t Level::stream_size (void) const
{
    size_t s (0);
    foreach (tilemap_t::const_iterator, i, m_Map)
	s += stream_size_of (*i);
    s += stream_size_of (m_nCrates);
    foreach (objvec_t::const_iterator, i, m_Crates)
	s += stream_size_of (*i);
    s += stream_size_of (m_Robot);
    return (s);
}

