/* font.cc
**
**	Implements VGA font class.
*/

#include "font.h"

Font font;

Font::Font (void)
: m_Data (),
  m_LetterSize (0),
  m_Width (0),
  m_Height (0)
{
}

void Font::read (istream& is)
{
    uint8_t w, h;
    is >> w >> h;
    Resize (w, h);
    is.read (m_Data.begin(), m_LetterSize * 256);
    is.align();
}

void Font::write (ostream& os) const
{
    os << uint8_t(m_Width) << uint8_t(m_Height);
    os.write (m_Data.begin(), m_LetterSize * 256);
    os.align();
}

size_t Font::stream_size (void) const
{
    return (Align (stream_size_of(uint8_t(m_Width)) +
		   stream_size_of(uint8_t(m_Height)) +
		   m_LetterSize * 256));
}

memblock::iterator Font::GetLetterStart (wchar_t c)
{
    return (m_Data.begin() + c * m_LetterSize);
}

void Font::ActivatePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] |= (1 << offset % 8);
}

void Font::DeactivatePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] &= (~(1 << offset % 8));
}

void Font::TogglePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] ^= (1 << offset % 8);
}

bool Font::ReadPoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    return (li [offset / 8] & (1 << offset % 8) != 0);
}

void Font::Resize (dim_t w, dim_t h)
{
    m_Width = w;
    m_Height = h;
    m_LetterSize = (m_Width * m_Height + 7) / 8;
    m_Data.resize (m_LetterSize * 256);
}

int Font::PrintString (CGC& gc, coord_t x, coord_t y, const char* s, color_t color)
{
    size_t m = x;
    for (uoff_t n = 0; n < strlen(s); ++ n)
	m += PrintCharacter (gc, m, y, s[n], color);
    return (m - x);
}

int Font::PrintCharacter (CGC& gc, coord_t x, coord_t y, wchar_t c, color_t color)
{
    memblock::const_iterator li = GetLetterStart (c);
    size_t bit = 0, cmw = 0;
    for (uoff_t j = 0; j < m_Height; ++ j) {
	for (uoff_t i = 0; i < m_Width; ++ i) {
	    if ((*li & (1 << bit)) > 0) {
		gc.SetPixel (Point(x + i, y + j), color);
		cmw = max (i, cmw);
	    }
	    if (bit < 7)
		++ bit;
	    else {
		bit = 0;
		++ li;
	    }
	}
    }
    return (cmw + 2);
}

