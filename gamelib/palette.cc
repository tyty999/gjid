/* palette.cc
**
**	Implements VGA palette class.
*/

#include <string.h>
#include <game/palette.h>
#include <game/graph.h>

static BYTE FadeBuffer [MAXCOL * 3];
static BYTE StandardPaletteType [MAXCOL * 3];

PaletteType :: PaletteType (void)
{
}

void PaletteType :: SetRGB (int ncol, int red, int green, int blue)
{
    colors [ncol * 3] = min (red, MAXBRIGHT);
    colors [ncol * 3 + 1] = min (green, MAXBRIGHT);
    colors [ncol * 3 + 2] = min (blue, MAXBRIGHT);
    SetPaletteColor (ncol, red, green, blue);
}

void PaletteType :: SetAll (BYTE *new_palette)
{
    memcpy (colors, new_palette, MAXCOL * 3);
    SetPaletteColors (0, MAXCOL, new_palette);
}

void PaletteType :: FadeOut (WORD Start, WORD End)
{
int lev, col, beam;
BYTE * curmap;

    -- End;
    memcpy (FadeBuffer, colors, MAXCOL * 3);
    for (lev = 0; lev < MAXBRIGHT; ++ lev) {
       curmap = colors;
       for (col = Start; col < End; ++ col) {
	  for (beam = 0; beam < 3; ++ beam)
	     if (curmap[beam] > 0)
		curmap[beam] -= 1;
	  curmap += 3;
       }
       SetAll (colors);
    }
}

void PaletteType :: FadeIn (WORD Start, WORD End)
{
int lev, col, beam;
BYTE *curmap;
BYTE *bufmap;

    -- End;
    for (lev = 0; lev < MAXBRIGHT; ++ lev) {
       curmap = colors;
       bufmap = FadeBuffer;
       for (col = Start; col < End; ++ col) {
	  for (beam = 0; beam < 3; ++ beam)
	     if (curmap[beam] < bufmap[beam])
		++ curmap[beam];
	  curmap += 3;
	  bufmap += 3;
       }
       SetAll (colors);
    }
}

void PaletteType :: RotateLeft (WORD Start, WORD End)
{
BYTE TempBuf[3];
    -- End;
    memcpy (TempBuf, colors + Start * 3, 3);
    memmove (colors + Start * 3, colors + (Start + 1) * 3, (End - Start) * 3);
    memcpy (colors + End * 3, TempBuf, 3);
    SetAll (colors);
}

void PaletteType :: RotateRight (WORD Start, WORD End)
{
BYTE TempBuf[3];
    -- End;
    memcpy (TempBuf, colors + End * 3, 3);
    memmove (colors + (Start + 1) * 3, colors + Start * 3, (End - Start) * 3);
    memcpy (colors + Start * 3, TempBuf, 3);
    SetAll (colors);
}

void PaletteType :: Reset (void)
{
    SetAll (StandardPaletteType);
}

void PaletteType :: Standardize (void)
{
    GetPaletteColors (0, MAXCOL, (PalID*) StandardPaletteType);
    memcpy (colors, StandardPaletteType, MAXCOL * 3);
}

void PaletteType :: Read (ifstream& is)
{
BYTE buffer [MAXCOL * 3];
    is.read (buffer, MAXCOL * 3 * sizeof(BYTE));
    SetAll (buffer);
}

void PaletteType :: Write (ofstream& os) const
{
    os.write (colors, MAXCOL * 3 * sizeof(BYTE));
}

PaletteType :: ~PaletteType (void)
{
}
