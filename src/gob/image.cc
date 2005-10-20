/// This file is part of the fbgl library.
///
/// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
/// This file is free software, distributed under the MIT License.
/// 
/// \file image.cc
///

#include "image.h"
#include "gif.h"

namespace fbgl {

/// Default constructor.
CImage::CImage (void)
: m_Pixels (),
  m_Palette (),
  m_Size (0, 0)
{
}

/// Resize the image to the given dimensions.
void CImage::Resize (dim_t w, dim_t h)
{
    m_Pixels.resize (w * h);
    m_Size[0] = w;
    m_Size[1] = h;
}

/// Reads the colormap from a GIF file.
void CImage::ReadGifColormap (istream& is, size_t bpp)
{
    m_Palette.resize (1 << bpp);
    foreach (fbgl::CPalette::iterator, i, m_Palette) {
	ray_t r, g, b;
	is >> r >> g >> b;
	*i = RGB(r,g,b);
    }
}

/// Reads the object from stream \p is.
void CImage::read (istream& is)
{
    using namespace gif;
    CFileHeader fh;
    if (is.remaining() < stream_size_of(fh))
	return;	// empty file; not really bad.
    is >> fh;
    if (fh.HasGlobalCmap())
	ReadGifColormap (is, fh.BitsPerPixel());

    uint8_t c = 0;
    while (is.remaining()) {
	is >> c;
	if (c == GIF_EXT_BLOCK_SIG) {
	    uint8_t extCode, blockSize = 0;
	    if (is.remaining() >= 2)
		is >> extCode >> blockSize;
	    while (blockSize) {
		is.skip (blockSize);
		is >> blockSize;
	    }
	} else if (c == GIF_IMAGE_BLOCK_SIG) {
	    CImageHeader ih;
	    if (is.remaining() < stream_size_of(ih))
		throw stream_bounds_exception ("lzw image header read", "gif", is.pos(), stream_size_of(ih), is.remaining());
	    is >> ih;
	    if (ih.m_Width > 16000 || ih.m_Height > 16000)
		throw runtime_error ("invalid image size");
	    Resize (ih.m_Width, ih.m_Height);
	    if (ih.HasLocalCmap())
		ReadGifColormap (is, ih.BitsPerPixel());
	    ostream os (begin(), Width() * Height());
	    CDecompressor d;
	    d.Run (is, os);
	} else if (c == GIF_END_OF_DATA_SIG)
	    break;
	else if (c)
	    throw runtime_error ("unrecognized block type found in the gif file");
    }
}

/// Writes the object to stream \p os.
void CImage::write (ostream&) const
{
}

/// Returns the size of the written object.
size_t CImage::stream_size (void) const
{
    return (Width() * Height());
}

} // namespace fbgl

