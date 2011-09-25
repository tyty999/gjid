// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "level.h"

//----------------------------------------------------------------------

Level::Level (void)
:_map (MAP_WIDTH * MAP_HEIGHT)
,_objects()
{
    _objects.push_back (ObjectType (0, 0, RobotNorthPix));
    fill (_map.begin(), _map.end(), tilemap_t::value_type(FloorPix));
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
    for (size_t i = 1; i < _objects.size(); ++ i)
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
    Robot().pic = dirImage [where];

    // Check if map square can be moved on
    if (!CanMoveTo (Robot().x + dx, Robot().y + dy, where))
	return;

    // Check if a crate needs to be moved
    int ciw = FindCrate (Robot().x + dx, Robot().y + dy);
    if (ciw >= 0) {
	// Can only move one crate - this checks for another one behind ciw
	//	also checks if the square behind crate can be moved into
	if (FindCrate (Robot().x + 2 * dx, Robot().y + 2 * dy) < 0 && CanMoveTo (Robot().x + 2 * dx, Robot().y + 2 * dy, where)) {
	    _objects[ciw].x += dx;
	    _objects[ciw].y += dy;
	    Robot().x += dx;
	    Robot().y += dy;
	    if (At(_objects[ciw].x, _objects[ciw].y) == DisposePix)
		DisposeCrate (ciw);
	}
    } else {
	Robot().x += dx;
	Robot().y += dy;
    }
}

void Level::MoveRobot (int x, int y, PicIndex pic)
{
    if (CanMoveTo (x, y, North)) {
	Robot().x = x;
	Robot().y = y;
	Robot().pic = pic;
    }
}

void Level::AddCrate (int x, int y, PicIndex pic)
{
    _objects.push_back (ObjectType (x, y, pic));
}

bool Level::Finished (void) const
{
    return (_objects.size() == 1 && At(Robot().x, Robot().y) == ExitPix);
}

const char* Level::Load (const char* ldata)
{
    static const char picToChar[NumberOfMapPics+1] = "0E.^v><#%+~!`   @NP";
    _objects.clear();
    _objects.push_back (ObjectType (0, 0, RobotNorthPix));
    for (int y = 0; y < MAP_HEIGHT; ++y) {
	for (int x = 0; x < MAP_WIDTH; ++x) {
	    char c = *ldata++;
	    const char* pf = strchr (picToChar, c);
	    PicIndex pic = pf ? PicIndex(distance(picToChar,pf)) : FloorPix;
	    if (pic >= RobotNorthPix) {
		if (pic >= Barrel1Pix)
		    _objects.push_back (ObjectType (x, y, pic));
		else
		    MoveRobot (x, y, pic);
	    	pic = FloorPix;
	    }
	    _map[y*MAP_WIDTH+x] = pic;
	}
    }
    return (*ldata ? ldata : NULL);
}

const char levels_data[] =
"~~~~###########~~~~~"
"~~~~#......0..#~~~~~"
"~~~~#.%.###^#.#~~~~~"
"~~~~#.%.....#.##~~~~"
"~~~~#..N.N.N#..#~~~~"
"~~~~#.%.NNN.#.@#~~~~"
"~~~~#.%N.N.N####~~~~"
"~~~~#.%.%%..E#~~~~~~"
"~~~~#.%.....##~~~~~~"
"~~~~#.......0#~~~~~~"
"~~~~##########~~~~~~"
"~~~~~~~~~~~~~~~~~~~~"

"!!!++++++++++%%%!!!!"
"!!!+.....@.....%!!!!"
"!+++P%%%%%%%++.%%!!!"
"!+.+.%.......+..%%%!"
"!+...>PNPNPN.++N..%!"
"!+.+.%N....P.0+.N.%!"
"!+.+.>N...N.+++N..%!"
"!+..0%PNPNP++++..0%!"
"!+++.%...........%%!"
"!+.+P%%%.EE+++++%%!!"
"!+.......+++!!!!!!!!"
"!++++++++++!!!!!!!!!"

"~~+++++++++~~~~~~~~~"
"~~+0..+...+++++~~~~~"
"~~+0..+.+.P...+++~~~"
"~~+0..+...P.0.>.+~~~"
"~~++P.%%%%%.P.+.+~~~"
"~~~+..%....^+++v+~~~"
"~~~+...PN..N.@.E+~~~"
"~~~+....v.+.++..+~~~"
"~~~++++.N.N..+N.+~~~"
"~~~~~~+++.++.<.N++~~"
"~~~~~~~~+....>N.0+~~"
"~~~~~~~~+++++++++++~"

"~~~~~~~~~~~~~~~~~~~~"
"~~~~~~~~~~~~~~~~~~~~"
"~~~~~~##########~~~~"
"~~~####....%.+E#~~~~"
"~~~#0...P.P....#~~~~"
"~~~#0.%.P..%.+.#~~~~"
"~~~#0.%@P.P.P..#~~~~"
"~~~#0.%....%.+.#~~~~"
"~~~#0...P.P.P..#~~~~"
"~~~######..%...#~~~~"
"~~~~~~~~########~~~~"
"~~~~~~~~~~~~~~~~~~~~"

"####%%#%%##%%%######"
"#E.....N..0........#"
"##++.++####v######.#"
"#@#+.+.............#"
"#.#+.+.###v##0#v#..#"
"#..+.+.#+....N.N#.0#"
"#.P....#.N++NN..#.##"
"%%%%.%....N....NNN.#"
"%....%...+++.P..#..#"
"%.N%%%..++.N.P.N<<.#"
"%0.N>...0+......>>>#"
"%%%%%%##+++#########"

"++++++++%%##########"
"+@......<.##.#....0#"
"+++..++.<....NN%%%.#"
"++....+0E%NNN%.%.N.#"
"++.++.+++%...%...N.#"
"++.0+.+0.%%v%%..N..#"
"%%%.......N.%..N.%0#"
"%....N++N.%.%.%.%%P#"
"#.P%N.N..N>..N.N...#"
"#.P....N..%P%P%.%%.#"
"#...#####.<.0.<.N..#"
"#####~~~####%%%#####"

"````````````````````"
"```++++++++`````````"
"```+.....E+++++++++`"
"`+++++.N..+.......+`"
"`+......+N+0.P..+.+`"
"`+..N+..N...P..@+.+`"
"`++P.>P+.+.++++++.+`"
"`+0.Pv..NN....N0+.+`"
"`++P.+.N..+.P...>.+`"
"``+......+++...++++`"
"``++++++++`+++++````"
"````````````````````"

"####################"
"#@................0#"
"#.#v###v##########N#"
"#.#P.N..N.##%%%#.>.#"
"#.#.###E#..#####.#v#"
"#.#............#...#"
"#...#.N.NNNN.N.0N#.#"
"#.#.####.#.#####.#.#"
"#.#......#.........#"
"#..v#########^###.N#"
"#.N....N....0.>...0#"
"####################"

"+%%..%%%%%%%%%%%%%%+"
"%0%%%%0%%...>0%..%E%"
"%NN>...%%..%%.NN.N0%"
"%....%....N...N.>..%"
"%.NN.%%%%.%%%%%%%v^%"
"%N.N.......>.......%"
"%@N..%.%%%^%%.%N.%N%"
"%..N.<..N.N.%.N.<..%"
"%.v%.%.N..N.%N<.%..%"
"%.v%0%%%%.%.%.%N.%.%"
"%.N..N....>.....0..%"
"+%%%%%%%%%%%%%%%%%%+"

"~~~~~~~~++++++++++~~"
"~~~~~~~~+.>......+~~"
"~~+++++++.+N+.N.N+~~"
"~~+..N....0.+++N.+~~"
"~~+..N.++++...>.0+~~"
"~~+.+v++.0<.P@++++~~"
"~~+0+......++++~~~~~"
"~++++++++..>.++~~~~~"
"~+0.....+..+P++~~~~~"
"~+<.N>N.++++..+++~~~"
"~+........E....0+~~~"
"~+++++++++++++++++~~"

"!!#########!!!!!!!!!"
"!!##.#@...######!!!!"
"!!##..N#..#.>.###!!!"
"!!#..P.0..>N.N###!!!"
"!!#.NPP..##P#...#!!!"
"!!#...P..>.P..#P#!!!"
"!#####...#N##.#.#!!!"
"!####...##......###!"
"!####.#.N..#.#....#!"
"!####...##0#E#.##.#!"
"!###########......#!"
"!!!!!!!!!!!########!"

"00000%%%%00%%%%%0000"
"00000%..%%%%..@%0000"
"00000%.E%0..<..%%000"
"000%%%.....%.NN.%000"
"000%%..N%%.%..N.%000"
"000%%.%...N..N..%000"
"000%..%^%%^%%0.%%000"
"000%.N..%...%%.%0000"
"000%%%.N...N.%.%0000"
"00000%0.%%%....%0000"
"00000%%%%0%%%%%%0000"
"00000000000000000000"

".........++++++....."
"...+++++++....+....."
"...+.....N.NE.+....."
"...+.+.0+++.+.+....."
"...+....+.+v+.+....."
"...++++.+.>N.0+....."
".....++N+^NNv++....."
".....+.N>...v.+....."
".....+.+++N++.+....."
".....+..@...N.+....."
".....+++...++++....."
".......++++++++....."

"##%%%%%%%%%%%%%%%%##"
"#.................0#"
"#.###.####.NN#####.#"
"#....N...#.N.....#.#"
"#.######.#...###.#.#"
"#.#.....N.##N.#E.#.#"
"#.#.#.N.@....N...N.#"
"#P#.####.#######.#.#"
"#.#.....N........#.#"
"#.######.###.#####.#"
"#0...P......N......#"
"##%%%%%%%%%%%%%%%###"
;
