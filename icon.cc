// icon.cc
//
// 	Implements the Icon class
//

#include "icon.h"
#include "targa.h"

Icon::Icon (dim_t w, dim_t h, const color_t* p)
: m_Bits (),
  m_Palette (),
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

void Icon::MergePaletteInto (CPalette&)
{
}

void Icon::read (istream& is)
{   
    string s (ICON_ID_STRING_LENGTH);
    is.read (s.begin(), ICON_ID_STRING_LENGTH);
    size_t w = 0, h = 0;
    if (s != ICON_ID_STRING || is.remaining() < stream_size_of(w) + stream_size_of(h))
	throw runtime_error ("no image data found");
    is >> w >> h;
    if (is.remaining() < w * h * sizeof(color_t))
	throw runtime_error ("image data is corrupt");
    m_Width = w;
    m_Height = h;
    m_Bits.resize (w * h);
    is.read (m_Bits.begin(), m_Bits.size() * sizeof(color_t));
    is.align();
}

void Icon::write (ostream& os) const
{   
    os.write (ICON_ID_STRING, ICON_ID_STRING_LENGTH);
    os << size_t(m_Width) << size_t(m_Height);
    os.write (m_Bits.begin(), m_Bits.size() * sizeof(color_t));
    os.align();
}

size_t Icon::stream_size (void) const
{
    return (Align (ICON_ID_STRING_LENGTH +
		   stream_size_of (size_t(m_Width)) +
		   stream_size_of (size_t(m_Height)) +
		   m_Bits.size() * sizeof(color_t)));
}

