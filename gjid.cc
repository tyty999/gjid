// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gjid.h"
#include <time.h>

#define char const char
#include "tileset.xpm"
#include "logo.xpm"
#undef char

//----------------------------------------------------------------------

MainApp (GJID)

//----------------------------------------------------------------------

GJID::GJID (void)
: CXApp()
,_state (state_Title)
,_storyPage (0)
,_level (0)
,_curLevel()
,_levels()
{
    CreateWindow ("GJID", 320, 240);
    _imgtiles = LoadImage (tileset_xpm);
    _imglogo = LoadImage (logo_xpm);
    const char* ldata = levels_data;
    while (ldata) {
	_levels.push_back (Level());
	ldata = _levels.back().Load (ldata);
    }
    _curLevel = _levels[0];
}

/*static*/ GJID& GJID::Instance (void)
{
    static GJID s_App;
    return (s_App);
}

void GJID::GoToState (EGameState state)
{
    _state = state;
    Update();
}

static const GJID::SImageTile c_Tiles [NumberOfPics] = {
    { 32, 48, 16, 16 },	// DisposePix
    { 48, 48, 16, 16 },	// ExitPix
    { 48, 16, 16, 16 },	// FloorPix
    { 16, 32, 16, 16 },	// OWDNorthPix
    { 32, 32, 16, 16 },	// OWDSouthPix
    {  0, 32, 16, 16 },	// OWDEastPix
    { 48, 32, 16, 16 },	// OWDWestPix
    {  0, 16, 16, 16 },	// Wall1Pix
    { 16, 16, 16, 16 },	// Wall2Pix
    { 32, 16, 16, 16 },	// Wall3Pix
    {  0, 64, 16, 16 },	// Back1Pix
    { 16, 64, 16, 16 },	// Back2Pix
    { 32, 64, 16, 16 },	// Back3Pix
    { 16,  0, 16, 16 },	// RobotNorthPix
    { 32,  0, 16, 16 },	// RobotSouthPix
    {  0,  0, 16, 16 },	// RobotEastPix
    { 48,  0, 16, 16 },	// RobotWestPix
    {  0, 48, 16, 16 },	// Barrel1Pix
    { 16, 48, 16, 16 },	// Barrel2Pix
    {  0,  0, 50, 50 },	// LogoGPix
    { 50,  0, 50, 50 },	// LogoJPix
    {100,  0, 50, 50 },	// LogoIPix
    {150,  0, 50, 50 }	// LogoDPix
};

inline void GJID::PutTile (PicIndex ti, int x, int y)
{
    DrawImageTile (ti < NumberOfMapPics ? _imgtiles : _imglogo, c_Tiles[ti], x, y);
}

//----------------------------------------------------------------------
// Tile screen helpers

void GJID::FillWithTile (PicIndex tidx)
{
    for (int y = 0; y < Height(); y += TILE_H)
	for (int x = 0; x < Width(); x += TILE_W)
	    PutTile (tidx, x, y);
}

void GJID::DecodeBitmapWithTile (const uint16_t* p, size_t n, PicIndex tidx)
{
    for (size_t y = 0; y < n; ++ y)
	for (size_t x = 0, mask = (1 << 15); x < 16; ++ x, mask >>= 1)
	    if (p[y] & mask)
		PutTile (tidx, (x + 2) * TILE_W, (y + 3) * TILE_H);
}

//----------------------------------------------------------------------
// Screen drawing

inline void GJID::IntroScreen (void)
{
    static const uint16_t title[] = {	// "GJID"
	0xEEEC, 0x824A, 0x824A, 0xA24A, 0xAA4A, 0xE6EC
    };
    FillWithTile (Back1Pix);
    DecodeBitmapWithTile (title, VectorSize(title), Wall2Pix);
}

inline void GJID::PrintStory (void)
{
    int x, y, row = 0;

    FillWithTile (Back1Pix);
    for (y = 0; y < Height(); y += TILE_H) {
	PutTile (Wall1Pix, 0, y);
	PutTile (Wall1Pix, Width() - TILE_W, y);
    }
    for (x = TILE_W; x < Width() - TILE_W; x += TILE_W) {
	PutTile (Wall1Pix, x, 0);
	PutTile (Wall1Pix, x, Height() - TILE_H);
    }
    DrawText (145, Height() - 9, "Hit any key", RGB(0,0,0));
    DrawText (144, Height() - 10, "Hit any key", RGB(128,128,128));

    if (_storyPage == 0) {
	PutTile (LogoGPix, 40, TILE_H * 2); 
	PutTile (LogoJPix, 100, TILE_H * 2); 
	PutTile (LogoIPix, 160, TILE_H * 2); 
	PutTile (LogoDPix, 220, TILE_H * 2); 
	row = 8;
    }
    if (_storyPage < 2) {
	static const char storyPage1[] =
	    "In the year 32333 AD two robot cities on the planet Nikarade were\0"
	    "arming themselves against each other. Both set up large complexes\0"
	    "in which powerful photon disruptors were stored. After many years\0"
	    "of increasing tension on of the cities elected another leader who\0"
	    "attempted to make peace with the enemy. The cities finally agreed\0"
	    "to an entente in 32407 and to recycle some of the created weapons\0"
	    " \0"
	    "The problem was that no robot wanted to go down into the dungeons\0"
	    "and accomplish this dangerous task. Finally, one robot named GJID\0"
	    "came forward to take up the job. He was a simple robot and little\0"
	    "to lose. Besides, there was a reward offered for the job :)\0";
	static const char storyPage2[] =
	    "In this game you play GJID, whose task is to move each crate into\0"
	    "recycling bins. At times complex mazes and one-way doors can make\0"
	    "this quite difficult. GJID is not very powerful and can only move\0"
	    "one crate at a time. Also, he can only push the crates, not pull.\0"
	    " \0"
	    "When you have recycled all the crates on the level you should use\0"
	    "the exit door to move on to the next. This weapons complex has 14\0"
	    "levels for you to clear.\0"
	    " \0"
	    "     Controls:   Cursor keys to move\0"
	    "                 F1  show this help\0"
	    "                 F6  restart the level\0"
	    "                 F8  skip the level\0"
	    "                 F10 quit the game\0";
	for (const char* l (_storyPage == 0 ? storyPage1 : storyPage2); strlen(l); ++row, l += strlen(l)+1)
	    DrawText (TILE_W*2, TILE_H*2 + (row+1)*8, l, RGB(128,128,0));
    } else if (_storyPage == 2) {
	x = TILE_W*2;
	y = TILE_H*2+7;
	DrawText (x+50, y, "Things you will find in the maze:", RGB(255,255,255));
	y += 17;
	static const PicIndex pic[] = { Barrel2Pix, Barrel1Pix, DisposePix, OWDEastPix, ExitPix };
	static const char* desc[] = { "- Photon disruptor", "- Nuclear weapon", "- Recycling bin", "- One-way door", "- The exit" };
	for (size_t i = 0; i < VectorSize(pic); ++ i) {
	    PutTile (pic[i], x, y);
	    DrawText (x+TILE_W*2, y+5, desc[i], RGB(128,128,0));
	    y += 17;
	}
    }
}

inline void GJID::DrawLevel (void)
{
    Level::tilemap_t::const_iterator it (_curLevel.Map().begin());
    FillWithTile (PicIndex(_curLevel.Map().back()));
    for (int y = 0; y < MAP_HEIGHT*TILE_H; y += TILE_H)
	for (int x = 0; x < MAP_WIDTH*TILE_W; x += TILE_W)
	    PutTile (PicIndex(*it++), x, y);
    foreach (Level::objvec_t::const_iterator, i, _curLevel.Objects())
	PutTile (PicIndex(i->pic), i->x*TILE_W, i->y*TILE_H);
}

inline void GJID::WinnerScreen (void)
{
    static const uint16_t title[] = {	// "WIN"
	0x45D2, 0x449A, 0x449E, 0x5496, 0x5492, 0x29D2
    };
    FillWithTile (Wall1Pix);
    DecodeBitmapWithTile (title, VectorSize(title), RobotNorthPix);
}

inline void GJID::LoserScreen (void)
{
    static const uint16_t title[] = {	// "LOSE"
	0x8F77, 0x8944, 0x8966, 0x8934, 0x8914, 0xEF77
    };
    FillWithTile (Back3Pix);
    DecodeBitmapWithTile (title, VectorSize(title), DisposePix);
}

void GJID::OnDraw (void)
{
    CXApp::OnDraw();
    switch (_state) {
	default:
	case state_Title:	return (IntroScreen());
	case state_Story:	return (PrintStory());
	case state_Game:	return (DrawLevel());
	case state_Winner:	return (WinnerScreen());
	case state_Loser:	return (LoserScreen());
    }
}

//----------------------------------------------------------------------
// Input handling

inline void GJID::TitleKeys (key_t key)
{
    GoToState (key == XK_Escape ? state_Game : state_Story);
}

inline void GJID::StoryKeys (key_t key)
{
    if (key == XK_Page_Up || key == XK_Up || key == ('b'|XKM_Ctrl))
	_storyPage -= !!_storyPage;
    else {
	++ _storyPage;
	if (_storyPage > 2 || key == XK_Escape) {
	    _storyPage = 2;
	    GoToState (state_Game);
	}
    }
    Update();
}

inline void GJID::LevelKeys (key_t key)
{
    switch (key) {
	case 'k':
	case XK_Up:	_curLevel.MoveRobot (North);	break;
	case 'j':
	case XK_Down:	_curLevel.MoveRobot (South);	break;
	case 'l':
	case XK_Right:	_curLevel.MoveRobot (East);	break;
	case 'h':
	case XK_Left:	_curLevel.MoveRobot (West);	break;
	case XK_F1:	GoToState (state_Story);	break;
	case 'q':
	case XK_Escape:	Quit();				break;
	case XK_F10:	GoToState (state_Loser);	break;
	case XK_F8:	_level = (_level + 1) % _levels.size();
	case XK_F6:	_curLevel = _levels [_level];	break;
    }
    if (_curLevel.Finished()) {
	++ _level;
	if (_level < _levels.size())
	    _curLevel = _levels [_level];
	else {
	    _level = 0;
	    GoToState (state_Winner);
	}
    }
    Update();
}

void GJID::OnKey (key_t key)
{
    switch (_state) {
	default:
	case state_Title:	return (TitleKeys (key));
	case state_Story:	return (StoryKeys (key));
	case state_Game:	return (LevelKeys (key));
	case state_Winner:
	case state_Loser:	Quit(); break;
    }
}
