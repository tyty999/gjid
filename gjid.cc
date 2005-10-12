// gjid.cc
//
//	Implements a puzzle game based on DOS shareware version.
//

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
  m_Levels (0),
  m_Story ()
{
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
	if (m_Story.empty() || m_Levels.empty()) {
	    LoadData (DATAFILE);
	    Update();
	    if (!m_Levels.empty())
		m_CurLevel = m_Levels[0];
	    if (m_Levels.empty() || m_Story.empty() || !m_EditedPackage.empty())
		GoToState (state_Editor);
	} else if (time(NULL) > titleDelay + 3)
	    GoToState (state_Story);
    }
}

void GJID::OnDraw (CGC& gc)
{
    CApplication::OnDraw (gc);
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
    gc.Palette() = m_Palette;
}

void GJID::OnKey (key_t key, keystate_t ks)
{
    typedef void (GJID::*pfnkey_t)(key_t key, keystate_t ks);
    static const pfnkey_t kfn [state_Last] = {
	&GJID::TitleKeys,	// state_Title
	&GJID::StoryKeys,	// state_Story
	&GJID::LevelKeys,	// state_Game
	&GJID::WinnerKeys,	// state_Winner
	&GJID::LoserKeys,	// state_Loser
	&GJID::EditorKeys	// state_Editor
    };
    (this->*kfn[m_State])(key, ks);
}

void GJID::IntroScreen (CGC& gc)
{
    coord_t x, y;
    static const int title [6][15] = {
	{1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0}};

    for (y = 0; y < 200; y += SQUARE_SIDE)
	for (x = 0; x < 320; x += SQUARE_SIDE)
	    m_Pics [Back1Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 15; ++ x)
	    if (title[y][x])
		m_Pics [Wall2Pix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::TitleKeys (key_t key, keystate_t)
{
    if (m_Story.empty())
	return;
    GoToState (key == key_Esc ? state_Game : state_Story);
}

void GJID::LoserScreen (CGC& gc)
{
    coord_t x, y;
    static const int title [6][16] = {
	{1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
	{1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1}};

    for (y = 0; y < 200; y += SQUARE_SIDE)
	for (x = 0; x < 320; x += SQUARE_SIDE)
	    m_Pics [Back3Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 16; ++ x)
	    if (title[y][x])
		m_Pics [DisposePix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::LoserKeys (key_t, keystate_t)
{
    Quit();
}

void GJID::PrintStory (CGC& gc)
{
    coord_t x, y, row = 0;

    gc.Clear (68);
    for (y = 0; y < 240; y += SQUARE_SIDE) {
	m_Pics [Wall1Pix].Put (gc, 0, y);
	m_Pics [Wall1Pix].Put (gc, 320 - SQUARE_SIDE, y);
    }
    for (x = SQUARE_SIDE; x < 320 - SQUARE_SIDE; x += SQUARE_SIDE) {
	m_Pics [Wall1Pix].Put (gc, x, 0);
	m_Pics [Wall1Pix].Put (gc, x, 240 - SQUARE_SIDE);
    }
    m_Font.PrintString (gc, 145, 231, "Hit any key", 16);
    m_Font.PrintString (gc, 144, 230, "Hit any key", 25);

    if (m_StoryPage == 0) {
	m_Pics[LogoGPix].PutMasked (gc, 40, SQUARE_SIDE * 2); 
	m_Pics[LogoJPix].PutMasked (gc, 100, SQUARE_SIDE * 2); 
	m_Pics[LogoIPix].PutMasked (gc, 160, SQUARE_SIDE * 2); 
	m_Pics[LogoDPix].PutMasked (gc, 220, SQUARE_SIDE * 2); 
	row = 8;

	string line;
	for (string::const_iterator ist = m_Story.begin(); ist != m_Story.end(); ++ ist) {
	    string::const_iterator iend = m_Story.find ('\n', ist);
	    line.assign (ist, iend);
	    ist = iend;
	    if (line == "$")
		break;
	    m_Font.PrintString (gc, SQUARE_SIDE * 2, SQUARE_SIDE * 2 + row * 7, line, 140);
	    ++ row;
	}
    } else if (m_StoryPage == 1) {
	string line;
	for (string::const_iterator ist = m_Story.find('$') + 2; ist != m_Story.end(); ++ ist) {
	    string::const_iterator iend = m_Story.find ('\n', ist);
	    line.assign (ist, iend);
	    ist = iend;
	    m_Font.PrintString (gc, SQUARE_SIDE * 2, SQUARE_SIDE * 2 + row * 7, line, 140);
	    ++ row;
	}
    } else if (m_StoryPage == 2) {
	x = SQUARE_SIDE * 2;
	y = SQUARE_SIDE * 2 + 7;
	m_Font.PrintString (gc, x + 50, y, "Things you will find in the maze:", 15);
	y += 17;
	m_Pics [DisposePix].Put (gc, x, y);
	m_Font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- A recycling bin", 140);
	y += 17;
	m_Pics [ExitPix].Put (gc, x, y);
	m_Font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- An exit door", 140);
	y += 17;
	m_Pics [Barrel1Pix].PutMasked (gc, x, y);
	m_Font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- Nuclear weapon", 140);
	y += 17;
	m_Pics [Barrel2Pix].PutMasked (gc, x, y);
	m_Font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- Photon disruptor", 140);
	y += 17;
	m_Pics [OWDNorthPix].Put (gc, x, y);
	m_Pics [OWDSouthPix].Put (gc, x + SQUARE_SIDE, y);
	m_Pics [OWDEastPix].Put (gc, x + SQUARE_SIDE * 2, y);
	m_Pics [OWDWestPix].Put (gc, x + SQUARE_SIDE * 3, y);
	m_Font.PrintString (gc, x + SQUARE_SIDE * 5, y + 5, "- One-way doors", 140);
    }
}

void GJID::StoryKeys (key_t key, keystate_t ks)
{
    if (key == key_PageUp || key == key_Up || (key == 'b' && ks[ks_Ctrl]))
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

void GJID::WinnerScreen (CGC& gc)
{
    coord_t x, y;
    static const int title [6][15] = {
	{0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1},
	{0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1},
	{0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1},
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1},
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1}};

    for (y = 0; y < 200; y += SQUARE_SIDE)
	for (x = 0; x < 320; x += SQUARE_SIDE)
	    m_Pics [Wall1Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 15; ++ x)
	    if (title[y][x])
		m_Pics [RobotNorthPix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::WinnerKeys (key_t, keystate_t)
{
    Quit();
}

void GJID::DrawLevel (CGC& gc)
{
    gc.Clear();
    m_CurLevel.Draw (gc, m_Pics);
}

void GJID::LevelKeys (key_t key, keystate_t)
{
    switch (key) {
	case key_Up:	m_CurLevel.MoveRobot (North);	break;
	case key_Down:	m_CurLevel.MoveRobot (South);	break;
	case key_Right:	m_CurLevel.MoveRobot (East);	break;
	case key_Left:	m_CurLevel.MoveRobot (West);	break;
	case key_F1:	GoToState (state_Story);	break;
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
    gc.Box (Rect (m_SelectedTile[0] * SQUARE_SIDE, m_SelectedTile[1] * SQUARE_SIDE, (m_SelectedTile[0] + 1) * SQUARE_SIDE - 1, (m_SelectedTile[1] + 1) * SQUARE_SIDE - 1), 15);
    for (uoff_t i = 0; i < NumberOfMapPics; ++ i)
	m_Pics[i].Put (gc, i * SQUARE_SIDE, gc.Height() - SQUARE_SIDE);
    gc.Box (Rect (m_SelectedPic * SQUARE_SIDE, gc.Height() - SQUARE_SIDE, (m_SelectedPic + 1) * SQUARE_SIDE - 1, gc.Height() - SQUARE_SIDE + SQUARE_SIDE - 1), 15);
}

void GJID::EditorKeys (key_t key, keystate_t)
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
	    SaveData ("gjid.dat");
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
    is >> m_Palette;
    is >> m_Font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	is >> m_Pics[i];
    is >> m_Story;
    is.align();
    is >> m_Levels;
}

void GJID::SaveData (const char* filename) const
{
    size_t dataSize = stream_size_of(m_Palette) + stream_size_of(m_Font);
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	dataSize += stream_size_of (m_Pics[i]);
    dataSize += Align (stream_size_of (m_Story));
    dataSize += stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Palette;
    os << m_Font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	os << m_Pics[i];
    os << m_Story;
    os.align();
    os << m_Levels;

    buf.write_file (filename);
}

