// icon.cc
//
// 	Implements the Icon class
//

#include "icon.h"

/// Default constructor.
Icon::Icon (dim_t w, dim_t h, const color_t* p)
: CImage ()
{
    SetImage (w, h, p);
}

/// Copies data from the given source.
void Icon::SetImage (dim_t w, dim_t h, const color_t* p)
{
    Resize (Size2d (w, h));
    if (p)
	copy_n (p, w * h, begin());
}

/// Reads the object from stream \p is.
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
    Resize (Size2d (w, h));
    nr_container_read (is, *this);
    is.align();
}

/// Writes the object to stream \p os.
void Icon::write (ostream& os) const
{
    os.write (ICON_ID_STRING, ICON_ID_STRING_LENGTH);
    os << size_t(Width()) << size_t(Height());
    nr_container_write (os, *this);
    os.align();
}

/// Returns the size of the written object.
size_t Icon::stream_size (void) const
{
    return (Align (ICON_ID_STRING_LENGTH +
		   stream_size_of (size_t(Width())) +
		   stream_size_of (size_t(Height())) +
		   nr_container_stream_size (*this)));
}

