/* palette.cc
**
**	Implements VGA palette class.
*/

#include <string.h>
#include "palette.h"
#include "graph.h"

static BYTE FadeBuffer [MAXCOL * 3];

PaletteType :: PaletteType (void)
{
}

void PaletteType :: SetAll (BYTE *new_palette)
{
    memcpy (colors, new_palette, MAXCOL * 3);
    SetPaletteColors (0, MAXCOL, new_palette);
}

void PaletteType :: FadeOut (WORD Start, WORD End)
{
    -- End;
    memcpy (FadeBuffer, colors, MAXCOL * 3);
    for (int lev = 0; lev < MAXBRIGHT; ++ lev) {
       BYTE* curmap = colors;
       for (WORD col = Start; col < End; ++ col) {
	  for (int beam = 0; beam < 3; ++ beam)
	     if (curmap[beam] > 0)
		curmap[beam] -= 1;
	  curmap += 3;
       }
       SetAll (colors);
    }
}

void PaletteType :: FadeIn (WORD Start, WORD End)
{
    -- End;
    for (int lev = 0; lev < MAXBRIGHT; ++ lev) {
       BYTE* curmap = colors;
       BYTE* bufmap = FadeBuffer;
       for (WORD col = Start; col < End; ++ col) {
	  for (int beam = 0; beam < 3; ++ beam)
	     if (curmap[beam] < bufmap[beam])
		++ curmap[beam];
	  curmap += 3;
	  bufmap += 3;
       }
       SetAll (colors);
    }
}

void PaletteType :: Read (ifstream& is)
{
    BYTE buffer [MAXCOL * 3];
    is.read ((char*) buffer, MAXCOL * 3 * sizeof(BYTE));
    SetAll (buffer);
}

void PaletteType :: Write (ofstream& os) const
{
    os.write ((const char*) colors, MAXCOL * 3 * sizeof(BYTE));
}

PaletteType :: ~PaletteType (void)
{
}
