/* level.cc
**
**	Implements the level class.
*/

#include <unistd.h>
#include "gjid.h"

Level :: Level (void)
{
int x, y;
    for (y = 0; y < MAP_HEIGHT; ++ y)
       for (x = 0; x < MAP_WIDTH; ++ x)
	  Map[x][y] = FloorPix;
    Robot.x = 0;
    Robot.y = 0;
    Robot.pic = RobotNorthPix;
    nCrates = 0;
}

Level& Level :: operator= (Level& ToBe)
{
int x, y;
ObjectType * NewCrate, * CurCrate;

    for (y = 0; y < MAP_HEIGHT; ++ y)
       for (x = 0; x < MAP_WIDTH; ++ x)
	  Map[x][y] = ToBe.Map[x][y];
    Robot.x = ToBe.Robot.x;
    Robot.y = ToBe.Robot.y;
    Robot.pic = ToBe.Robot.pic;
    nCrates = ToBe.nCrates;
    
    while (!Crates.IsEmpty()) {
       Crates.Head();
       Crates.Delete();
    }

    ToBe.Crates.Head();
    for (x = 0; x < nCrates; ++ x) {
       CurCrate = ToBe.Crates.LookAt();
       ToBe.Crates.Next();
       NewCrate = new ObjectType;
       NewCrate->x = CurCrate->x;
       NewCrate->y = CurCrate->y;
       NewCrate->pic = CurCrate->pic;
       Crates.Tail();
       Crates.InsertAfter (NewCrate);
    }
    Crates.Head();

    return (*this);
}

void Level :: Draw (WORD x, WORD y)
{
Icon combo;
ObjectType * cc;
int i;

    pics [Map [x][y]].Put (x * SQUARE_SIDE, y * SQUARE_SIDE);
    Crates.Head();
    for (i = 0; i < nCrates; ++ i) {
       cc = Crates.LookAt();
       if (cc->x == x && cc->y == y) {
	  combo = pics [cc->pic];
	  combo.BlendWith (pics [FloorPix], SeeThroughBlend);
          combo.Put (x * SQUARE_SIDE, y * SQUARE_SIDE);
	  break;	// One crate per block
       }
       Crates.Next();
    }
    if (Robot.x == x && Robot.y == y) {
       combo = pics [Robot.pic];
       combo.BlendWith (pics [Map [x][y]], SeeThroughBlend);
       combo.Put (x * SQUARE_SIDE, y * SQUARE_SIDE);
    }
}

void Level :: Draw (void)
{
WORD x, y;
    for (y = 0; y < MAP_HEIGHT; ++ y)
       for (x = 0; x < MAP_WIDTH; ++ x)
	  Draw (x, y);
}

BOOL Level :: CanMoveTo (WORD x, WORD y, RobotDir where)
{
BOOL Can = FALSE;

    switch (Map[x][y]) {
       case DisposePix:
       case ExitPix:
       case FloorPix:		Can = TRUE; break;
       case OWDNorthPix:	if (where == North)
       				   Can = TRUE;
				break;
       case OWDSouthPix:	if (where == South)
       				   Can = TRUE;
				break;
       case OWDEastPix:		if (where == East)
       				   Can = TRUE;
				break;
       case OWDWestPix:		if (where == West)
       				   Can = TRUE;
				break;
       default: 		break;
    }

    return (Can);
}

int Level :: FindCrate (WORD x, WORD y)
{
ObjectType * cc;
int i;

    Crates.Head();
    for (i = 0; i < nCrates; ++ i) {
       cc = Crates.LookAt();
       Crates.Next();
       if (cc->x == x && cc->y == y)
	  return (i);
    }
    return (-1);
}

void Level :: SetCell (WORD x, WORD y, PicIndex pic)
{
    Map [x][y] = pic;
}

BOOL Level :: Finished (void)
{
    return (nCrates == 0 && Map [Robot.x][Robot.y] == ExitPix);
}

void Level :: MoveRobot (RobotDir where)
{
int dx = 0, dy = 0;
int ciw;
ObjectType * CurCrate;

    switch (where) {
        case North:	dy = -1;
			Robot.pic = RobotNorthPix;
			break;
        case South:	dy = 1;
			Robot.pic = RobotSouthPix;
			break;
        case East:	dx = 1;
			Robot.pic = RobotEastPix;
			break;
        case West:	dx = -1;
			Robot.pic = RobotWestPix;
			break;
    }

    // Check if map square can be moved on
    if (!CanMoveTo (Robot.x + dx, Robot.y + dy, where)) {
       Draw (Robot.x, Robot.y);
       return;
    }

    // Check if a crate needs to be moved
    ciw = FindCrate (Robot.x + dx, Robot.y + dy);
    if (ciw < 0) {
       Robot.x += dx;
       Robot.y += dy;
       Draw (Robot.x - dx, Robot.y - dy);
       Draw (Robot.x, Robot.y);
    }
    else {
       // Can only move one crate - this checks for another one behind ciw
       //	also checks if the square behind crate can be moved into
       if (FindCrate (Robot.x + 2 * dx, Robot.y + 2 * dy) < 0 &&
	   CanMoveTo (Robot.x + 2 * dx, Robot.y + 2 * dy, where)) {
	  Crates.Head();
	  Crates.Skip (ciw);
	  CurCrate = Crates.LookAt();
	  CurCrate->x += dx;
	  CurCrate->y += dy;
	  Robot.x += dx;
	  Robot.y += dy;
	  Draw (Robot.x - dx, Robot.y - dy);
	  Draw (Robot.x, Robot.y);
	  Draw (Robot.x + dx, Robot.y + dy);

	  if (Map [CurCrate->x][CurCrate->y] == DisposePix) {
	     usleep (100);
	     DisposeCrate (ciw);
	     Draw (Robot.x + dx, Robot.y + dy);
	  }
       }
    }
}

void Level :: MoveRobot (WORD x, WORD y, PicIndex pic)
{
WORD oldx, oldy;

    if (CanMoveTo (x, y, North)) {
       oldx = Robot.x;
       oldy = Robot.y;
       Robot.x = x;
       Robot.y = y;
       Robot.pic = pic;
       Draw (oldx, oldy);
       Draw (Robot.x, Robot.y);
    }
}

void Level :: DisposeCrate (WORD index)
{
    Crates.Head();
    Crates.Skip (index);
    Crates.Delete();
    -- nCrates;
}

void Level :: AddCrate (WORD x, WORD y, PicIndex pic)
{
ObjectType * NewCrate;

    NewCrate = new ObjectType;
    NewCrate->x = x;
    NewCrate->y = y;
    NewCrate->pic = pic;
    Crates.Tail();
    Crates.InsertAfter (NewCrate);
    ++ nCrates;
}

void Level :: Read (ifstream& is)
{
ObjectType * NewCrate;
int i;

    is.read (Map, MAP_HEIGHT * MAP_WIDTH * sizeof(PicIndex));
    is.read (&nCrates, sizeof(WORD));
    for (i = 0; i < nCrates; ++ i) {
       NewCrate = new ObjectType;
       is.read (NewCrate, sizeof(ObjectType));
       Crates.Tail();
       Crates.InsertAfter (NewCrate);
    }
    is.read (&Robot, sizeof(ObjectType));
}

void Level :: Write (ofstream& os)
{
int i;
    os.write (Map, MAP_HEIGHT * MAP_WIDTH * sizeof(PicIndex));
    os.write (&nCrates, sizeof(WORD));
    Crates.Head();
    for (i = 0; i < nCrates; ++ i) {
       os.write (Crates.LookAt(), sizeof(ObjectType));
       Crates.Next();
    }
    os.write (&Robot, sizeof(ObjectType));
}

Level :: ~Level (void)
{
}

