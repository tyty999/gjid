/* gjid.cc
**
**	Implements a puzzle game based on DOS shareware version.
*/

#include <graph.h>
#include <icon.h>
#include <font.h>
#include <unistd.h>
#include "gjid.h"

Icon pics [NumberOfPics];
PaletteType pal;
LList<Level> levels;
WORD nLevels;
char * story;
WORD StorySize = 0;

int main (int argc, char**)
{
    GraphInit(0);

    cout << "Using data file " << DATAFILE << " ...\n";
    LoadData (DATAFILE);

    if (argc < 2)
       PlayGame();
    else
       EditData();

    GraphClose();
    return (0);
}

void IntroScreen (void)
{
int x, y;
int title [6][15] = {
	{1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0}};

    pal.FadeOut();    
    for (y = 0; y < 200; y += SQUARE_SIDE)
       for (x = 0; x < 320; x += SQUARE_SIDE)
          pics [Back1Pix].Put (x, y);

    for (y = 0; y < 6; ++ y)
       for (x = 0; x < 15; ++ x)
	  if (title[y][x])
	     pics [Wall2Pix].Put ((x + 2) * SQUARE_SIDE, 
	     			    (y + 3) * SQUARE_SIDE);

    pal.FadeIn();
    sleep (1);
}

void LoserScreen (void)
{
int x, y;
int title [6][16] = {
	{1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0},
	{1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
	{1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1}};

    pal.FadeOut();
    for (y = 0; y < 200; y += SQUARE_SIDE)
       for (x = 0; x < 320; x += SQUARE_SIDE)
          pics [Back3Pix].Put (x, y);

    for (y = 0; y < 6; ++ y)
       for (x = 0; x < 16; ++ x)
	  if (title[y][x])
	     pics [DisposePix].Put ((x + 2) * SQUARE_SIDE, 
	     			    (y + 3) * SQUARE_SIDE);

    pal.FadeIn();
    GetKeyWait();
}

void PrintStory (void)
{
WORD x, y, i, offset, row, key = 0;
ifstream is;
char buffer[80], c;
Icon blend, backgr (50, 50);
    
    pal.FadeOut();
    ClearScreen (68);
    for (y = 0; y < 200; y += SQUARE_SIDE) {
       pics [Wall1Pix].Put (0, y);
       pics [Wall1Pix].Put (304, y);
    }
    for (x = 0; x < 320; x += SQUARE_SIDE) {
       pics [Wall1Pix].Put (x, 0);
       pics [Wall1Pix].Put (x, 184);
    }

    font.PrintString (145, 191, "Hit any key", 16);
    font.PrintString (144, 190, "Hit any key", 25);

    SetClipWindow (SQUARE_SIDE, SQUARE_SIDE, 
    		   320 - SQUARE_SIDE, 200 - SQUARE_SIDE);

    backgr.Get (40, SQUARE_SIDE * 2);
    blend = pics [LogoGPix];
    blend.BlendWith (backgr, SeeThroughBlend);
    blend.Put (40, SQUARE_SIDE * 2); 
    backgr.Get (100, SQUARE_SIDE * 2);
    blend = pics [LogoJPix];
    blend.BlendWith (backgr, SeeThroughBlend);
    blend.Put (100, SQUARE_SIDE * 2); 
    backgr.Get (160, SQUARE_SIDE * 2);
    blend = pics [LogoIPix];
    blend.BlendWith (backgr, SeeThroughBlend);
    blend.Put (160, SQUARE_SIDE * 2); 
    backgr.Get (220, SQUARE_SIDE * 2);
    blend = pics [LogoDPix];
    blend.BlendWith (backgr, SeeThroughBlend);
    blend.Put (220, SQUARE_SIDE * 2); 

    row = 8;
    offset = 0;
    while (offset < StorySize) {
       i = 0;
       while ((c = story [offset++]) != '\n' && offset < StorySize) {
	  buffer[i] = c;
	  ++ i;
       }
       buffer[i] = '\x0';

       if (strcmp (buffer, "$") == 0) {
	  pal.FadeIn();
	  if ((key = GetKeyWait()) == KEY_ESC)
	     break;
	  pal.FadeOut();
	  ClearScreen (68);
	  row = 0;
       }
       else
          font.PrintString (SQUARE_SIDE * 2, SQUARE_SIDE * 2 + row * 7, 
			    buffer, 140);
       ++ row;
    }

    if (key != KEY_ESC) {
       pal.FadeIn();
       GetKeyWait();
       pal.FadeOut();

       ClearScreen (68);
       x = SQUARE_SIDE * 2;
       y = SQUARE_SIDE * 2 + 7;
       font.PrintString (x + 50, y, "Things you will find in the maze:", 15);
       y += 17;
       pics [DisposePix].Put (x, y);
       font.PrintString (x + SQUARE_SIDE * 2, y + 5, "- A recycling bin", 140);
       y += 17;
       pics [ExitPix].Put (x, y);
       font.PrintString (x + SQUARE_SIDE * 2, y + 5, "- An exit door", 140);
       y += 17;
       pics [Barrel1Pix].Put (x, y);
       font.PrintString (x + SQUARE_SIDE * 2, y + 5, "- Nuclear weapon", 140);
       y += 17;
       pics [Barrel2Pix].Put (x, y);
       font.PrintString (x + SQUARE_SIDE * 2, y + 5, "- Photon disruptor", 140);
       y += 17;
       pics [OWDNorthPix].Put (x, y);
       pics [OWDSouthPix].Put (x + SQUARE_SIDE, y);
       pics [OWDEastPix].Put (x + SQUARE_SIDE * 2, y);
       pics [OWDWestPix].Put (x + SQUARE_SIDE * 3, y);
       font.PrintString (x + SQUARE_SIDE * 5, y + 5, "- One-way doors", 140);
       pal.FadeIn();
       GetKeyWait();
    }

    SetClipWindow (0, 0, 320, 200);

    // We shall not need the story from now on
    if (story != NULL)
       delete [] story;
}

void WinnerScreen (void)
{
int x, y;
int title [6][15] = {
	{0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1},
	{0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1},
	{0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1},
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1},
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1}};

    pal.FadeOut();
    for (y = 0; y < 200; y += SQUARE_SIDE)
       for (x = 0; x < 320; x += SQUARE_SIDE)
          pics [Wall1Pix].Put (x, y);

    for (y = 0; y < 6; ++ y)
       for (x = 0; x < 15; ++ x)
	  if (title[y][x])
	     pics [RobotNorthPix].Put ((x + 2) * SQUARE_SIDE, 
	     			    (y + 3) * SQUARE_SIDE);

    pal.FadeIn();
    GetKeyWait();
}

void PlayGame (void)
{
int key = 0;
Level CurLevel;

    if (nLevels <= 0)
       return;

    IntroScreen();
    PrintStory();

    pal.FadeOut();
    ClearScreen (0);
    levels.Head();
    CurLevel = *(levels.LookAt());
    CurLevel.Draw();
    pal.FadeIn();

    do {
       switch (key) {
	  case KEY_UP:		CurLevel.MoveRobot (North);
	  			break;
	  case KEY_DOWN:	CurLevel.MoveRobot (South);
	  			break;
	  case KEY_RIGHT:	CurLevel.MoveRobot (East);
	  			break;
	  case KEY_LEFT:	CurLevel.MoveRobot (West);
	  			break;
	  case KEY_F6:		CurLevel = *(levels.LookAt());
				pal.FadeOut();
				CurLevel.Draw();
				pal.FadeIn();
				break;
	  case KEY_F8:		levels.Next();
	  			if (!levels.OnList())
				   levels.Head();
				CurLevel = *(levels.LookAt());
				pal.FadeOut();
				CurLevel.Draw();
				pal.FadeIn();
				break;
       }
       if (CurLevel.Finished()) {
	  levels.Next();
	  if (levels.OnList()) {
             CurLevel = *(levels.LookAt());
	     pal.FadeOut();
             CurLevel.Draw();
	     pal.FadeIn();
	  }
       }
    }
    while (levels.OnList() && (key = GetKeyWait()) != KEY_F10);

    if (key == KEY_F10)
       LoserScreen();
    else
       WinnerScreen();
    pal.FadeOut();
}

void EditData (void)
{
BOOL Done = FALSE;
Level * CurLevel, * NewLevel;
PicIndex CurPic = Wall1Pix;
int x = 0, y = 0, i, key;
int crate;
#define KEY_NEXTLEVEL	(int)('n')
#define KEY_PREVLEVEL	(int)('p')
#define KEY_QUIT	(int)('q')
#define KEY_SAVE	(int)('s')
#define KEY_PAINT	(int)(' ')
#define KEY_TILE_CYCLE	(int)('\t')

    EnableClipping();
    ClearScreen (0);
    levels.Head();
    if (levels.OnList())
       CurLevel = levels.LookAt();
    else {
       NewLevel = new Level;
       levels.Tail();
       levels.InsertAfter (NewLevel);
       levels.Tail();
       CurLevel = levels.LookAt();
       ++ nLevels;
    }

    do {
	SetClipWindow (0, 0, 320, 183);
	CurLevel->Draw();
	DrawRect (x * SQUARE_SIDE, y * SQUARE_SIDE, 
		  (x + 1) * SQUARE_SIDE - 1, (y + 1) * SQUARE_SIDE - 1, 15);
	SetClipWindow (0, 0, 320, 200);

	for (i = 0; i < NumberOfMapPics; ++ i)
	   pics[i].Put (i * SQUARE_SIDE, 184);
	DrawRect (CurPic * SQUARE_SIDE, 184, 
		  (CurPic + 1) * SQUARE_SIDE - 1, 184 + SQUARE_SIDE - 1, 15);

	key = GetKeyWait();
	switch (key) {
	    case KEY_NEXTLEVEL:
		levels.Next();
		if (levels.OnList())
		    CurLevel = levels.LookAt();
		else {
		    NewLevel = new Level;
		    levels.Tail();
		    levels.InsertAfter (NewLevel);
		    levels.Tail();
		    CurLevel = levels.LookAt();
		    ++ nLevels;
		}
		break;
	    case KEY_QUIT:
		Done = TRUE;
		break;
	    case KEY_PREVLEVEL:
		levels.Prev();
		if (levels.OnList())
		    CurLevel = levels.LookAt();
		else {
		    levels.Head();
		    CurLevel = levels.LookAt();
		}
		break;
	    case KEY_SAVE:
		SaveData ("gjid.dat");
		break;
	    case KEY_LEFT:	if (x > 0) -- x;		break;
	    case KEY_RIGHT:	if (x < MAP_WIDTH - 1) ++ x;	break;
	    case KEY_UP:	if (y > 0) -- y;		break;
	    case KEY_DOWN:	if (y < MAP_WIDTH - 1) ++ y;	break;
	    case KEY_PAINT:
		switch (CurPic) {
		    case Barrel1Pix:
		    case Barrel2Pix:
			if ((crate = CurLevel->FindCrate (x, y)) < 0)
			    CurLevel->AddCrate (x, y, CurPic);
			else
			    CurLevel->DisposeCrate (crate);
			break;
		    case RobotNorthPix:
		    case RobotSouthPix:
		    case RobotEastPix:
		    case RobotWestPix:
			CurLevel->MoveRobot (x, y, CurPic);
			break;
		    default:
			CurLevel->SetCell (x, y, CurPic);
			break;
		}
		break;
	    case KEY_TILE_CYCLE:
		CurPic = PicIndex ((CurPic + 1) % NumberOfMapPics);
		break;
	};
    }
    while (!Done);
}

