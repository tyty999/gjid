// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gjid.h"
#include <time.h>

//----------------------------------------------------------------------

FbglMain (GJID)

//----------------------------------------------------------------------

GJID::GJID (void)
: CApplication (),
  m_State (state_Title),
  m_EditedPackage (),
  m_StoryPage (0),
  m_Level (0),
  m_CurLevel (),
  m_SelectedPic (Wall1Pix),
  m_SelectedTile (0, 0),
  m_Font (),
  m_Pics (),
  m_Palette (),
  m_Levels (0)
{
    m_Palette.AllocColor (0,0,0);
}

/*static*/ GJID& GJID::Instance (void)
{
    static GJID s_App;
    return (s_App);
}

void GJID::OnCreate (void)
{
    CApplication::OnCreate();
    SetMode (stdmode_320x240x8);
}

void GJID::ProcessArguments (argc_t argc, argv_t argv)
{
    CApplication::ProcessArguments (argc, argv);
    if (argc >= 2)
	m_EditedPackage = argv[1];
}

void GJID::GoToState (EGameState state)
{
    m_State = state;
    Update();
}

void GJID::OnIdle (void)
{
    CApplication::OnIdle();
    if (m_State == state_Title) {
	static const time_t titleDelay (time (NULL));
	if (m_Levels.empty()) {
	    LoadData (DATAFILE);
	    Update();
	    if (!m_Levels.empty())
		m_CurLevel = m_Levels[0];
	    if (m_Levels.empty() || !m_EditedPackage.empty())
		GoToState (state_Editor);
	} else if (time(NULL) > titleDelay + 3)
	    GoToState (state_Story);
    }
}

void GJID::OnDraw (CGC& gc)
{
    CApplication::OnDraw (gc);
    if (m_Pics.empty())
	return;
    gc.Palette() = m_Palette;
    typedef void (GJID::*pfndraw_t)(CGC& gc);
    static const pfndraw_t dfn [state_Last] = {
	&GJID::IntroScreen,	// state_Title
	&GJID::PrintStory,	// state_Story
	&GJID::DrawLevel,	// state_Game
	&GJID::WinnerScreen,	// state_Winner
	&GJID::LoserScreen,	// state_Loser
	&GJID::DrawEditor	// state_Editor
    };
    (this->*dfn[m_State])(gc);
}

void GJID::OnKey (key_t key)
{
    typedef void (GJID::*pfnkey_t)(key_t key);
    static const pfnkey_t kfn [state_Last] = {
	&GJID::TitleKeys,	// state_Title
	&GJID::StoryKeys,	// state_Story
	&GJID::LevelKeys,	// state_Game
	&GJID::WinnerKeys,	// state_Winner
	&GJID::LoserKeys,	// state_Loser
	&GJID::EditorKeys	// state_Editor
    };
    (this->*kfn[m_State])(key);
}

void GJID::FillWithTile (CGC& gc, PicIndex tidx) const
{
    for (dim_t y = 0; y < gc.Height(); y += TILE_H)
	for (dim_t x = 0; x < gc.Width(); x += TILE_W)
	    m_Pics [tidx].Put (gc, x, y);
}

void GJID::DecodeBitmapWithTile (CGC& gc, const uint16_t* p, size_t n, PicIndex tidx) const
{
    for (uoff_t y = 0; y < n; ++ y)
	for (uoff_t x = 0, mask = (1 << 15); x < 16; ++ x, mask >>= 1)
	    if (p[y] & mask)
		m_Pics [tidx].Put (gc, (x + 2) * TILE_W, (y + 3) * TILE_H);
}

void GJID::IntroScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "GJID"
	0xEEEC, 0x824A, 0x824A, 0xA24A, 0xAA4A, 0xE6EC
    };
    FillWithTile (gc, Back1Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), Wall2Pix);
}

void GJID::TitleKeys (key_t key)
{
    GoToState (key == key_Esc ? state_Game : state_Story);
}

void GJID::WinnerScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "WIN"
	0x45D2, 0x449A, 0x449E, 0x5496, 0x5492, 0x29D2
    };
    FillWithTile (gc, Wall1Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), RobotNorthPix);
}

void GJID::WinnerKeys (key_t)
{
    Quit();
}

void GJID::LoserScreen (CGC& gc)
{
    static const uint16_t title[] = {	// "LOSE"
	0x8F77, 0x8944, 0x8966, 0x8934, 0x8914, 0xEF77
    };
    FillWithTile (gc, Back3Pix);
    DecodeBitmapWithTile (gc, title, VectorSize(title), DisposePix);
}

void GJID::LoserKeys (key_t)
{
    Quit();
}

void GJID::PrintStory (CGC& gc)
{
    coord_t x, y, row = 0;

    gc.Clear (gc.AllocColor (0,0,64));
    for (y = 0; y < gc.Height(); y += TILE_H) {
	m_Pics [Wall1Pix].Put (gc, 0, y);
	m_Pics [Wall1Pix].Put (gc, gc.Width() - TILE_W, y);
    }
    for (x = TILE_W; x < gc.Width() - TILE_W; x += TILE_W) {
	m_Pics [Wall1Pix].Put (gc, x, 0);
	m_Pics [Wall1Pix].Put (gc, x, gc.Height() - TILE_H);
    }
    m_Font.PrintString (gc, 145, gc.Height() - 9, "Hit any key", gc.AllocColor (0,0,0));
    m_Font.PrintString (gc, 144, gc.Height() - 10, "Hit any key", gc.AllocColor (128,128,128));

    if (m_StoryPage == 0) {
	m_Pics[LogoGPix].PutMasked (gc, 40, TILE_H * 2); 
	m_Pics[LogoJPix].PutMasked (gc, 100, TILE_H * 2); 
	m_Pics[LogoIPix].PutMasked (gc, 160, TILE_H * 2); 
	m_Pics[LogoDPix].PutMasked (gc, 220, TILE_H * 2); 
	row = 8;
    }
    if (m_StoryPage < 2) {
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
	const string storyPage (m_StoryPage == 0 ? storyPage1 : storyPage2);
	for (uoff_t i = 0; i < storyPage.size(); i += line.size() + 1) {
	    line.assign (storyPage.iat (i), storyPage.iat (storyPage.find ('\n', i)));
	    m_Font.PrintString (gc, TILE_W * 2, TILE_H * 2 + (row + 1) * 7, line, gc.AllocColor (128,128,0));
	    ++ row;
	}
    } else if (m_StoryPage == 2) {
	x = TILE_W * 2;
	y = TILE_H * 2 + 7;
	m_Font.PrintString (gc, x + 50, y, "Things you will find in the maze:", gc.AllocColor(255,255,255));
	y += 17;
	static const PicIndex pic[] = { DisposePix, ExitPix, Barrel1Pix, Barrel2Pix };
	static const char* desc[] = { "- A recycling bin", "- An exit door", "- Nuclear weapon", "- Photon disruptor" };
	for (uoff_t i = 0; i < VectorSize(pic); ++ i) {
	    m_Pics [pic[i]].PutMasked (gc, x, y);
	    m_Font.PrintString (gc, x + TILE_W * 2, y + 5, desc[i], gc.AllocColor(128,128,0));
	    y += 17;
	}
	m_Pics [OWDNorthPix].Put (gc, x, y);
	m_Pics [OWDSouthPix].Put (gc, x += TILE_W, y);
	m_Pics [OWDEastPix].Put (gc, x += TILE_W, y);
	m_Pics [OWDWestPix].Put (gc, x += TILE_W, y);
	m_Font.PrintString (gc, x += TILE_W * 2, y + 5, "- One-way doors", gc.AllocColor(128,128,0));
    }
}

void GJID::StoryKeys (key_t key)
{
    if (key == key_PageUp || key == key_Up || key == ('b'|ks_Ctrl))
	m_StoryPage -= !!m_StoryPage;
    else {
	++ m_StoryPage;
	if (m_StoryPage > 2 || key == key_Esc) {
	    m_StoryPage = 2;
	    GoToState (state_Game);
	}
    }
    Update();
}

void GJID::DrawLevel (CGC& gc)
{
    gc.Clear();
    m_CurLevel.Draw (gc, m_Pics);
}

void GJID::LevelKeys (key_t key)
{
    switch (key) {
	case 'k':
	case key_Up:	m_CurLevel.MoveRobot (North);	break;
	case 'j':
	case key_Down:	m_CurLevel.MoveRobot (South);	break;
	case 'l':
	case key_Right:	m_CurLevel.MoveRobot (East);	break;
	case 'h':
	case key_Left:	m_CurLevel.MoveRobot (West);	break;
	case key_F1:	GoToState (state_Story);	break;
	case 'q':
	case key_Esc:	Quit();				break;
	case key_F10:	GoToState (state_Loser);	break;
	case key_F8:	m_Level = (m_Level + 1) % m_Levels.size();
	case key_F6:	m_CurLevel = m_Levels [m_Level];	break;
    }
    if (m_CurLevel.Finished()) {
	++ m_Level;
	if (m_Level < m_Levels.size())
	    m_CurLevel = m_Levels [m_Level];
	else {
	    m_Level = 0;
	    GoToState (state_Winner);
	}
    }
    Update();
}

void GJID::DrawEditor (CGC& gc)
{
    gc.Clear();
    m_CurLevel.Draw (gc, m_Pics);
    for (uoff_t i = 0; i < NumberOfMapPics; ++ i)
	m_Pics[i].Put (gc, i * TILE_W, gc.Height() - TILE_H);
    const Point strtl (m_SelectedTile * TILE_W);
    const Point sprtl (m_SelectedPic * TILE_W, gc.Height() - TILE_H);
    const color_t white (gc.AllocColor (255,255,255));
    gc.Box (Rect (strtl, strtl + TILE_W - 1), white);
    gc.Box (Rect (sprtl, sprtl + TILE_W - 1), white);
}

void GJID::EditorKeys (key_t key)
{
    switch (key) {
	case key_F10:
	case 'q':
	    Quit();
	    break;
	case key_F8:
	case 'n':
	    if (m_Level >= m_Levels.size())
		m_Level = 0;
	    else {
		m_Levels [m_Level] = m_CurLevel;
		++ m_Level;
	    }
	    if (m_Level >= m_Levels.size())
		m_Levels.push_back();
	    m_CurLevel = m_Levels [m_Level];
	    break;
	case key_F7:
	case 'p':
	    if (m_Level < m_Levels.size())
		m_Levels [m_Level] = m_CurLevel;
	    m_Level -= !!m_Level;
	    break;
	case key_F2:
	case 's':
	    m_Levels [m_Level] = m_CurLevel;
	    SaveData ("data/levels.dat");
	    break;
	case key_Left:	if (m_SelectedTile[0] > 0) -- m_SelectedTile[0];		break;
	case key_Right:	if (m_SelectedTile[0] < MAP_WIDTH - 1) ++ m_SelectedTile[0];	break;
	case key_Up:	if (m_SelectedTile[1] > 0) -- m_SelectedTile[1];		break;
	case key_Down:	if (m_SelectedTile[1] < MAP_HEIGHT - 1) ++ m_SelectedTile[1];	break;
	case key_Space:
	case key_Enter:
	    switch (m_SelectedPic) {
		case Barrel1Pix:
		case Barrel2Pix: {
		    int crate;
		    if ((crate = m_CurLevel.FindCrate (m_SelectedTile[0], m_SelectedTile[1])) < 0)
			m_CurLevel.AddCrate (m_SelectedTile[0], m_SelectedTile[1], m_SelectedPic);
		    else
			m_CurLevel.DisposeCrate (crate);
		    break; }
		case RobotNorthPix:
		case RobotSouthPix:
		case RobotEastPix:
		case RobotWestPix:
		    m_CurLevel.MoveRobot (m_SelectedTile[0], m_SelectedTile[1], m_SelectedPic);
		    break;
		default:
		    m_CurLevel.SetCell (m_SelectedTile[0], m_SelectedTile[1], m_SelectedPic);
		    break;
	    }
	    break;
	case key_Tab:
	    m_SelectedPic = PicIndex ((m_SelectedPic + 1) % NumberOfMapPics);
	    break;
    }
    Update();
}

void GJID::LoadData (const char* filename)
{
    memblock buf;
    buf.read_file (filename);
    istream is (buf);
    is >> m_Font;
    is >> m_Pics;
    if (m_Pics.size() != NumberOfPics)
	throw runtime_error ("not enough tile pictures in the data file");
    is >> m_Levels;
    foreach (picvec_t::iterator, i, m_Pics)
	i->MergePaletteInto (m_Palette);
}

void GJID::SaveData (const char* filename) const
{
    const size_t dataSize = stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Levels;

    buf.write_file (filename);
}
