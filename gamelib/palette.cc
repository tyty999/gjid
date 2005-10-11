/* palette.cc
**
**	Implements VGA palette class.
*/

#include "palette.h"

CPalette PaletteType::s_FadeBuffer;

PaletteType::PaletteType (void)
: CPalette ()
{
}

void PaletteType::SetAll (const ray_t* p)
{
    for (uoff_t i = 0; i < MAXCOL; ++ i)
	Set (i, p[i*3], p[i*3+1], p[i*3+2]);
}

void PaletteType::FadeOut (color_t first, color_t last)
{
    for (uoff_t i = first; i < last; ++ i) {
	ray_t r, g, b;
	Get (i, r, g, b);
	s_FadeBuffer.Set (i, r, g, b);
    }
    for (uoff_t i = 0; i < MAXCOL; ++ i)
	Set (i, 0, 0, 0);
}

void PaletteType::FadeIn (color_t first, color_t last)
{
    for (uoff_t i = first; i < last; ++ i) {
	ray_t r, g, b;
	s_FadeBuffer.Get (i, r, g, b);
	Set (i, r, g, b);
    }
}

