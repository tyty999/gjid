/* font.cc
**
**	Implements VGA font class.
*/

#define FONT_UNIT
#include <game/font.h>
#include <game/graph.h>

#ifndef NO_FONT
Font font;
#endif

Font :: Font (void)
{
    width = 0;
    height = 0;
    data = NULL;
}

void Font :: Read (ifstream& is)
{
BYTE w, h;
    is.read (&w, sizeof(BYTE));
    is.read (&h, sizeof(BYTE));
    Resize (w, h);
    is.read (data, LetterSize * NOL);
}

void Font :: Write (ofstream& os) const
{
    os.write (&width, sizeof(BYTE));
    os.write (&height, sizeof(BYTE));
    os.write (data, LetterSize * NOL);
}

BYTE * Font :: GetLetterStart (WORD letter)
{
    return (&data [letter * LetterSize]);
}

void Font :: ActivatePoint (WORD letter, WORD xpos, WORD ypos)
{
BYTE * curlet = NULL;
WORD offset;

    curlet = GetLetterStart (letter);
    offset = ypos * width + xpos;
    curlet [offset / 8] |= (1 << offset % 8);
}

void Font :: DeactivatePoint (WORD letter, WORD xpos, WORD ypos)
{
BYTE *curlet = NULL;
WORD offset;

    curlet = GetLetterStart (letter);
    offset = ypos * width + xpos;
    curlet [offset / 8] &= (~(1 << offset % 8));
}

void Font :: TogglePoint (WORD letter, WORD xpos, WORD ypos)
{
BYTE *curlet = NULL;
WORD offset;

    curlet = GetLetterStart (letter);
    offset = ypos * width + xpos;
    curlet [offset / 8] ^= (1 << offset % 8);
}


BOOL Font :: ReadPoint (WORD letter, WORD xpos, WORD ypos)
{
BYTE *curlet = NULL;
WORD offset;

    curlet = GetLetterStart (letter);
    offset = ypos * width + xpos;
    return (curlet [offset / 8] & (1 << offset % 8) != 0);
}

void Font :: Resize (BYTE NewWidth, BYTE NewHeight)
{
#define FIT_IN_BYTES(x)	(((x) % 8 == 0) ? ((x) / 8) : ((x) / 8 + 1))

    width = NewWidth;
    height = NewHeight;
    LetterSize = FIT_IN_BYTES(width * height);

    if (data != NULL)
       delete [] data;
    data = new BYTE [LetterSize * NOL];
}

int Font :: PrintString (WORD x, WORD y, char * string, WORD color)
{
int n, m;
    m = x;
    for (n = 0;n < strlen (string); ++ n)
       m += PrintCharacter (m, y, (int) string[n], color);
    return (m - x);
}

int Font :: PrintCharacter (WORD x, WORD y, char ascii, WORD color)
{
BYTE * curlet;        	// Pointer to the letter bitmap.
int xpos,ypos,pos = 0;  // Cycle variables.
int mpos = 0;           // Maximum character width

    curlet = GetLetterStart (ascii);

    for (ypos = 0; ypos < height; ++ ypos) {
       for (xpos = 0; xpos < width; ++ xpos) {
	  if ((*curlet & (1 << pos)) > 0) {
	     SetPixel (x + xpos, y + ypos, color);
	     if (mpos < xpos)
		 mpos = xpos;
	  }
	  if (pos < 7)
	     ++ pos;
	  else {
	     pos = 0;
	     ++ curlet;
	  }
       }
    }

    return (mpos + 2);
}

Font :: ~Font (void)
{
}
