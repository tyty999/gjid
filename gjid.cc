// gjid.cc
//
//	Implements a puzzle game based on DOS shareware version.
//

#include <font.h>
#include "gjid.h"

picvec_t pics;
PaletteType pal;
vector<Level> levels;
size_t nLevels = 0;
string story;
size_t StorySize = 0;

class GJID : public CApplication {
public:
    static GJID&	Instance (void);
    virtual void	ProcessArguments (argc_t argc, argv_t argv);
protected:
			GJID (void);
    virtual void	OnIdle (void);
    virtual void	OnDraw (CGC& gc);
    virtual void	OnKey (key_t key, keystate_t ks);
private:
    enum EGameState {
	state_Title,
	state_Story,
	state_Game,
	state_Winner,
	state_Loser,
	state_Editor,
	state_Last
    };
private:
    void		GoToState (EGameState state);
    void		IntroScreen (CGC& gc);
    void		LoserScreen (CGC& gc);
    void		PrintStory (CGC& gc);
    void		WinnerScreen (CGC& gc);
    void		DrawLevel (CGC& gc);
    void		DrawEditor (CGC& gc);
    void		TitleKeys (key_t key, keystate_t ks);
    void		StoryKeys (key_t key, keystate_t ks);
    void		LevelKeys (key_t key, keystate_t ks);
    void		WinnerKeys (key_t key, keystate_t ks);
    void		LoserKeys (key_t key, keystate_t ks);
    void		EditorKeys (key_t key, keystate_t ks);
private:
    EGameState		m_State;
    string		m_EditedPackage;
    uoff_t		m_StoryPage;
    uoff_t		m_Level;
    Level		m_CurLevel;
    PicIndex		m_SelectedPic;
    Point		m_SelectedTile;
};

FbglMain (GJID)

GJID::GJID (void)
: CApplication (),
  m_State (state_Title),
  m_EditedPackage (),
  m_StoryPage (0),
  m_Level (0),
  m_CurLevel (),
  m_SelectedPic (Wall1Pix),
  m_SelectedTile (0, 0)
{
}

/*static*/ GJID& GJID::Instance (void)
{
    static GJID s_App;
    return (s_App);
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
	cout << "Using data file " << DATAFILE << " ...\n";
	LoadData (DATAFILE);
	if (!levels.empty())
	    m_CurLevel = levels[0];
	if (levels.empty() || story.empty() || !m_EditedPackage.empty())
	    GoToState (state_Editor);
	else
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
}

void GJID::OnKey (key_t key, keystate_t ks)
{
    CApplication::OnKey (key, ks);
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
	    pics [Back1Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 15; ++ x)
	    if (title[y][x])
		pics [Wall2Pix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::TitleKeys (key_t key, keystate_t)
{
    if (story.empty())
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
	    pics [Back3Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 16; ++ x)
	    if (title[y][x])
		pics [DisposePix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::LoserKeys (key_t, keystate_t)
{
    Quit();
}

void GJID::PrintStory (CGC& gc)
{
    coord_t x, y, i, row = 0;
    uoff_t offset = 0;
    char buffer[80], c;
    Icon blend, backgr (50, 50);

    if (m_StoryPage == 0) {
	gc.Clear (68);
	for (y = 0; y < 200; y += SQUARE_SIDE) {
	    pics [Wall1Pix].Put (gc, 0, y);
	    pics [Wall1Pix].Put (gc, 304, y);
	}
	for (x = 0; x < 320; x += SQUARE_SIDE) {
	    pics [Wall1Pix].Put (gc, x, 0);
	    pics [Wall1Pix].Put (gc, x, 184);
	}

	font.PrintString (gc, 145, 191, "Hit any key", 16);
	font.PrintString (gc, 144, 190, "Hit any key", 25);

	backgr.Get (gc, 40, SQUARE_SIDE * 2);
	blend = pics [LogoGPix];
	blend.BlendWith (backgr, SeeThroughBlend);
	blend.Put (gc, 40, SQUARE_SIDE * 2); 
	backgr.Get (gc, 100, SQUARE_SIDE * 2);
	blend = pics [LogoJPix];
	blend.BlendWith (backgr, SeeThroughBlend);
	blend.Put (gc, 100, SQUARE_SIDE * 2); 
	backgr.Get (gc, 160, SQUARE_SIDE * 2);
	blend = pics [LogoIPix];
	blend.BlendWith (backgr, SeeThroughBlend);
	blend.Put (gc, 160, SQUARE_SIDE * 2); 
	backgr.Get (gc, 220, SQUARE_SIDE * 2);
	blend = pics [LogoDPix];
	blend.BlendWith (backgr, SeeThroughBlend);
	blend.Put (gc, 220, SQUARE_SIDE * 2); 
	row = 8;

	while (offset < StorySize) {
	    i = 0;
	    while ((c = story [offset++]) != '\n' && offset < StorySize) {
		buffer[i] = c;
		++ i;
	    }
	    buffer[i] = '\x0';

	    if (strcmp (buffer, "$") == 0)
		break;
	    font.PrintString (gc, SQUARE_SIDE * 2, SQUARE_SIDE * 2 + row * 7, buffer, 140);
	    ++ row;
	}
    } else if (m_StoryPage == 1) {
	gc.Clear (68);
	offset = distance (story.begin(), story.find ('$')) + 1;
	while (offset < StorySize) {
	    i = 0;
	    while ((c = story [offset++]) != '\n' && offset < StorySize) {
		buffer[i] = c;
		++ i;
	    }
	    buffer[i] = '\x0';

	    if (strcmp (buffer, "$") == 0)
		break;
	    else
		font.PrintString (gc, SQUARE_SIDE * 2, SQUARE_SIDE * 2 + row * 7, buffer, 140);
	    ++ row;
	}
    } else if (m_StoryPage == 2) {
	gc.Clear (68);
	x = SQUARE_SIDE * 2;
	y = SQUARE_SIDE * 2 + 7;
	font.PrintString (gc, x + 50, y, "Things you will find in the maze:", 15);
	y += 17;
	pics [DisposePix].Put (gc, x, y);
	font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- A recycling bin", 140);
	y += 17;
	pics [ExitPix].Put (gc, x, y);
	font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- An exit door", 140);
	y += 17;
	pics [Barrel1Pix].Put (gc, x, y);
	font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- Nuclear weapon", 140);
	y += 17;
	pics [Barrel2Pix].Put (gc, x, y);
	font.PrintString (gc, x + SQUARE_SIDE * 2, y + 5, "- Photon disruptor", 140);
	y += 17;
	pics [OWDNorthPix].Put (gc, x, y);
	pics [OWDSouthPix].Put (gc, x + SQUARE_SIDE, y);
	pics [OWDEastPix].Put (gc, x + SQUARE_SIDE * 2, y);
	pics [OWDWestPix].Put (gc, x + SQUARE_SIDE * 3, y);
	font.PrintString (gc, x + SQUARE_SIDE * 5, y + 5, "- One-way doors", 140);
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
	    pics [Wall1Pix].Put (gc, x, y);

    for (y = 0; y < 6; ++ y)
	for (x = 0; x < 15; ++ x)
	    if (title[y][x])
		pics [RobotNorthPix].Put (gc, (x + 2) * SQUARE_SIDE, (y + 3) * SQUARE_SIDE);
}

void GJID::WinnerKeys (key_t, keystate_t)
{
    Quit();
}

void GJID::DrawLevel (CGC& gc)
{
    gc.Clear();
    levels[m_Level].Draw (gc);
}

void GJID::LevelKeys (key_t key, keystate_t)
{
    switch (key) {
	case key_Up:	m_CurLevel.MoveRobot (North);	break;
	case key_Down:	m_CurLevel.MoveRobot (South);	break;
	case key_Right:	m_CurLevel.MoveRobot (East);	break;
	case key_Left:	m_CurLevel.MoveRobot (West);	break;
	case key_F10:	GoToState (state_Loser);	break;
	case key_F8:	m_Level = (m_Level + 1) % levels.size();
	case key_F6:	m_CurLevel = levels [m_Level];	break;
    }
    if (m_CurLevel.Finished()) {
	++ m_Level;
	if (m_Level < levels.size())
	    m_CurLevel = levels [m_Level];
	else {
	    m_Level = 0;
	    GoToState (state_Winner);
	}
    }
    Update();
}

void GJID::DrawEditor (CGC& gc)
{
    levels[m_Level].Draw (gc);
    gc.Box (Rect (m_SelectedTile[0] * SQUARE_SIDE, m_SelectedTile[1] * SQUARE_SIDE, (m_SelectedTile[0] + 1) * SQUARE_SIDE - 1, (m_SelectedTile[1] + 1) * SQUARE_SIDE - 1), 15);
    for (uoff_t i = 0; i < NumberOfMapPics; ++ i)
	pics[i].Put (gc, i * SQUARE_SIDE, 184);
    gc.Box (Rect (m_SelectedPic * SQUARE_SIDE, 184, (m_SelectedPic + 1) * SQUARE_SIDE - 1, 184 + SQUARE_SIDE - 1), 15);
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
	    if (m_Level >= levels.size())
		m_Level = 0;
	    else {
		levels [m_Level] = m_CurLevel;
		++ m_Level;
	    }
	    if (m_Level >= levels.size()) {
		levels.push_back();
		++ nLevels;
	    }
	    m_CurLevel = levels [m_Level];
	    break;
	case key_F7:
	case 'p':
	    if (m_Level < levels.size())
		levels [m_Level] = m_CurLevel;
	    m_Level -= !!m_Level;
	    break;
	case key_F2:
	case 's':
	    levels [m_Level] = m_CurLevel;
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
}

