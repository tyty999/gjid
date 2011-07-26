// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gjid.h"
#include "cpio.h"
#include <time.h>

//----------------------------------------------------------------------

FbglMain (GJID)

//----------------------------------------------------------------------

GJID::GJID (void)
: CApp ()
,_state (state_Title)
,_storyPage (0)
,_level (0)
,_curLevel ()
,_font ()
,_pics ()
,_palette ()
,_levels (0)
{
    _palette.AllocColor (0,0,0);
    CreateWindow ("GJID", 320, 240);
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

void GJID::OnIdle (void)
{
    CApp::OnIdle();
    if (_state == state_Title) {
	static const time_t titleDelay (time (NULL));
	if (_levels.empty()) {
	    LoadData (DATAFILE);
	    Update();
	    if (_levels.empty())
		Quit();
	    else
		_curLevel = _levels[0];
	} else if (time(NULL) > titleDelay + 1)
	    GoToState (state_Story);
    }
}

void GJID::LoadData (const char* filename)
{
    CPIO datafile (filename);

    istream fntstm = datafile.File ("default.fnt"); fntstm >> _font;
    istream lvlstm = datafile.File ("levels.dat"); lvlstm >> _levels;

    _pics.resize (NumberOfPics);
    static const char c_PicFiles[] =
	"dispose.gif\0" "exit.gif\0" "floor.gif\0"
	"oneway_n.gif\0" "oneway_s.gif\0" "oneway_e.gif\0" "oneway_w.gif\0"
	"wall1.gif\0" "wall2.gif\0" "wall3.gif\0" "back1.gif\0" "back2.gif\0" "back3.gif\0"
	"robot_n.gif\0" "robot_s.gif\0" "robot_e.gif\0" "robot_w.gif\0"
	"barrel1.gif\0" "barrel2.gif\0"
	"logo_g.gif\0" "logo_j.gif\0" "logo_i.gif\0" "logo_d.gif\0";
    const char* picfilename = c_PicFiles;
    foreach (picvec_t::iterator, i, _pics) {
	istream picstm = datafile.File (picfilename);
	picstm >> *i;
	i->MergePaletteInto (_palette);
	picfilename += strlen(picfilename)+1;
    }
}

//----------------------------------------------------------------------
// Tile screen helpers

void GJID::FillWithTile (CGC& gc, PicIndex tidx) const
{
    for (dim_t y = 0; y < gc.Height(); y += TILE_H)
	for (dim_t x = 0; x < gc.Width(); x += TILE_W)
	    gc.Image (_pics[tidx], x, y);
}

void GJID::DecodeBitmapWithTile (CGC& gc, const uint16_t* p, size_t n, PicIndex tidx) const
{
    for (uoff_t y = 0; y < n; ++ y)
	for (uoff_t x = 0, mask = (1 << 15); x < 16; ++ x, mask >>= 1)
	    if (p[y] & mask)
		gc.Image (_pics [tidx], (x + 2) * TILE_W, (y + 3) * TILE_H);
}

//----------------------------------------------------------------------
// Screen drawing

inline void GJID::IntroScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "GJID"
	0xEEEC, 0x824A, 0x824A, 0xA24A, 0xAA4A, 0xE6EC
    };
    FillWithTile (gc, Back1Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), Wall2Pix);
}

inline void GJID::PrintStory (CGC& gc)
{
    coord_t x, y, row = 0;

    gc.Clear (gc.AllocColor (0,0,64));
    for (y = 0; y < gc.Height(); y += TILE_H) {
	gc.Image (_pics [Wall1Pix], 0, y);
	gc.Image (_pics [Wall1Pix], gc.Width() - TILE_W, y);
    }
    for (x = TILE_W; x < gc.Width() - TILE_W; x += TILE_W) {
	gc.Image (_pics [Wall1Pix], x, 0);
	gc.Image (_pics [Wall1Pix], x, gc.Height() - TILE_H);
    }
    _font.PrintString (gc, 145, gc.Height() - 9, "Hit any key", gc.AllocColor (0,0,0));
    _font.PrintString (gc, 144, gc.Height() - 10, "Hit any key", gc.AllocColor (128,128,128));

    if (_storyPage == 0) {
	gc.ImageMasked (_pics[LogoGPix], 40, TILE_H * 2); 
	gc.ImageMasked (_pics[LogoJPix], 100, TILE_H * 2); 
	gc.ImageMasked (_pics[LogoIPix], 160, TILE_H * 2); 
	gc.ImageMasked (_pics[LogoDPix], 220, TILE_H * 2); 
	row = 8;
    }
    if (_storyPage < 2) {
	static const char storyPage1[] =
	    "In the year 32333 AD two robot cities on the planet Nikarad were arming\n"
	    "themselves against each other. The both set up large complexes in which\n"
	    "powerful photon disrupters were stored. After many years of increasing\n"
	    "tension on of the cities elected another leader who attempted to make\n"
	    "peace with the enemy. The cities finally agreed for an entente to start\n"
	    "in 32407 and to recycle some of the created weapons.\n"
	    "\n"
	    "The problem was that no robot wanted to go down into the dungeons and\n"
	    "accomplish this dangerous task. Finally, one robot named GJID came\n"
	    "forward and offered to help. He was a simple robot and had not much to\n"
	    "lose. Besides, there was a reward offered for the job :)";
	static const char storyPage2[] =
	    "You play GJID in this game as he carefully moves the weapon crates into\n"
	    "recycling bins. Complex mazes and one-way doors make this quite difficult\n"
	    "at times. Unfortunately, GJID is not very powerful and can only move one\n"
	    "crate at a time. Furthermore, he can only push the crates, not pull.\n"
	    "\n"
	    "When you have recycled all the crates on the level you should use the\n"
	    "exit door to move on to the next. In this first mission you will go\n"
	    "through 14 levels.\n"
	    "\n"
	    "     Controls:   Cursor keys to move\n"
	    "                 F1   show this help\n"
	    "                 F6   restart the level\n"
	    "                 F8   skip the level\n"
	    "                 F10 quit the game";
	string line;
	const string storyPage (_storyPage == 0 ? storyPage1 : storyPage2);
	for (uoff_t i = 0; i < storyPage.size(); i += line.size() + 1) {
	    line.assign (storyPage.iat (i), storyPage.iat (storyPage.find ('\n', i)));
	    _font.PrintString (gc, TILE_W * 2, TILE_H * 2 + (row + 1) * 7, line, gc.AllocColor (128,128,0));
	    ++ row;
	}
    } else if (_storyPage == 2) {
	x = TILE_W * 2;
	y = TILE_H * 2 + 7;
	_font.PrintString (gc, x + 50, y, "Things you will find in the maze:", gc.AllocColor(255,255,255));
	y += 17;
	static const PicIndex pic[] = { DisposePix, ExitPix, Barrel1Pix, Barrel2Pix };
	static const char* desc[] = { "- A recycling bin", "- An exit door", "- Nuclear weapon", "- Photon disruptor" };
	for (uoff_t i = 0; i < VectorSize(pic); ++ i) {
	    gc.ImageMasked (_pics [pic[i]], x, y);
	    _font.PrintString (gc, x + TILE_W * 2, y + 5, desc[i], gc.AllocColor(128,128,0));
	    y += 17;
	}
	gc.Image (_pics [OWDNorthPix], x, y);
	gc.Image (_pics [OWDSouthPix], x += TILE_W, y);
	gc.Image (_pics [OWDEastPix], x += TILE_W, y);
	gc.Image (_pics [OWDWestPix], x += TILE_W, y);
	_font.PrintString (gc, x += TILE_W * 2, y + 5, "- One-way doors", gc.AllocColor(128,128,0));
    }
}

inline void GJID::DrawLevel (CGC& gc)
{
    gc.Clear();
    _curLevel.Draw (gc, _pics);
}

inline void GJID::WinnerScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "WIN"
	0x45D2, 0x449A, 0x449E, 0x5496, 0x5492, 0x29D2
    };
    FillWithTile (gc, Wall1Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), RobotNorthPix);
}

inline void GJID::LoserScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "LOSE"
	0x8F77, 0x8944, 0x8966, 0x8934, 0x8914, 0xEF77
    };
    FillWithTile (gc, Back3Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), DisposePix);
}

void GJID::OnDraw (CGC& gc)
{
    CApp::OnDraw (gc);
    if (_pics.empty())
	return;
    gc.Palette() = _palette;
    switch (_state) {
	default:
	case state_Title:	return (IntroScreen (gc));
	case state_Story:	return (PrintStory (gc));
	case state_Game:	return (DrawLevel (gc));
	case state_Winner:	return (WinnerScreen (gc));
	case state_Loser:	return (LoserScreen (gc));
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
