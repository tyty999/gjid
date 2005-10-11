// icon.cc
//
// 	Implements the Icon class
//

#include "icon.h"

Icon::Icon (dim_t w, dim_t h, const color_t* p)
: m_Bits (),
  m_Width (),
  m_Height ()
{
    SetImage (w, h, p);
}

void Icon::SetImage (dim_t w, dim_t h, const color_t* p)
{
    m_Width = w;
    m_Height = h;
    m_Bits.resize (w * h);
    if (p)
	copy_n (p, m_Bits.size(), m_Bits.begin());
}

void Icon::BlendWith (const Icon& v, BlendType how)
{
    if (m_Width != v.m_Width || m_Height != v.m_Height)
	return;

    if (how == SeeThroughBlend) {
	for (uoff_t i = 0; i < m_Bits.size(); ++ i)
	    if (m_Bits[i] == 0)
		m_Bits[i] = v.m_Bits[i];
    } else if (how == AndBlend) {
	for (uoff_t i = 0; i < m_Bits.size(); ++ i)
	    m_Bits[i] &= v.m_Bits[i];
    } else if (how == XorBlend) {
	for (uoff_t i = 0; i < m_Bits.size(); ++ i)
	    m_Bits[i] ^= v.m_Bits[i];
    } else if (how == OrBlend) {
	for (uoff_t i = 0; i < m_Bits.size(); ++ i)
	    m_Bits[i] |= v.m_Bits[i];
    }
}

void Icon::read (istream& is)
{   
    string s (ICON_ID_STRING_LENGTH);
    is.read (s.begin(), ICON_ID_STRING_LENGTH);
    dim_t w, h;
    if (s != ICON_ID_STRING || is.remaining() < stream_size_of(w) + stream_size_of(h))
	throw runtime_error ("no image data found");
    is >> w >> h;
    if (is.remaining() < w * h * sizeof(color_t))
	throw runtime_error ("image data is corrupt");
    m_Width = w;
    m_Height = h;
    m_Bits.resize (w * h);
    is.read (m_Bits.begin(), m_Bits.size() * sizeof(color_t));
}

void Icon::write (ostream& os) const
{   
    os.write (ICON_ID_STRING, ICON_ID_STRING_LENGTH);
    os << m_Width << m_Height;
    os.write (m_Bits.begin(), m_Bits.size() * sizeof(color_t));
}

size_t Icon::stream_size (void) const
{
    return (ICON_ID_STRING_LENGTH +
	    stream_size_of (m_Width) +
	    stream_size_of (m_Height) +
	    m_Bits.size() * sizeof(color_t));
}

void Icon::Put (CGC& gc, coord_t x, coord_t y) const
{
    gc.Image (Rect (x, y, x + Width(), y + Height()), m_Bits.begin());
}

void Icon::Get (CGC& gc, coord_t x, coord_t y)
{
    gc.GetImage (Rect (x, y, x + Width(), y + Height()), m_Bits.begin());
}

void Icon::SetPixel (coord_t x, coord_t y, color_t c)
{
    GetRow(y)[x] = c;
}

color_t Icon::GetPixel (coord_t x, coord_t y) const
{
    return (GetRow (y)[x]);
}

void Icon::SetRow (coord_t row, const color_t* p)
{
    copy_n (p, Width(), GetRow(row));
}

void Icon::SetCol (coord_t col, const color_t* p)
{
    vector<color_t>::iterator dest (m_Bits.begin() + col);
    for (uoff_t i = 0; i < Height(); ++i, ++p, dest+=Width())
	*dest = *p;
}

