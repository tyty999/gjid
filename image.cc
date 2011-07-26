// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "image.h"
#include "gif.h"

//----------------------------------------------------------------------
// Memory management.
//----------------------------------------------------------------------

/// Default constructor.
CImage::CImage (void)
: m_Pixels ()
, m_Palette ()
, m_Width (0)
, m_Height (0)
{
}

/// Default constructor.
CImage::CImage (dim_t w, dim_t h, const color_t* p)
{
    Resize (w, h);
    if (p)
	copy_n (p, w * h, m_Pixels.begin());
}

/// Resize the image to the given dimensions.
void CImage::Resize (dim_t w, dim_t h)
{
    m_Pixels.resize (w * h);
    m_Width = w;
    m_Height = h;
}

//----------------------------------------------------------------------
// Palette operations.
//----------------------------------------------------------------------

/// Maps the image colors onto \p pal. Call after reading, before drawing.
void CImage::MergePaletteInto (CPalette& pal)
{
    assert (!m_Palette.empty() && "This image doesn't have a palette to merge");
    foreach (CPalette::iterator, i, m_Palette)
	setRayA (*i, pal.AllocColor(*i));
    foreach (pixvec_t::iterator, i, m_Pixels)
	*i = getRayA (m_Palette [min (*i, m_Palette.size()-1)]);
}

//----------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------

/// Reads the colormap from a GIF file.
void CImage::ReadGifColormap (istream& is, size_t bpp)
{
    m_Palette.resize (1 << bpp);
    foreach (CPalette::iterator, i, m_Palette) {
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
	if (c == GIF_EXT_BLOCK_SIG && is.remaining() >= 2) {
	    uint8_t extCode, blockSize = 0;
	    is >> extCode;
	    if (extCode == GIF_EXT_GC_SIG) {
		CGraphicsControl gch;
		is >> gch;
	    } else {
		is >> blockSize;
		while (blockSize) {
		    is.skip (blockSize);
		    is >> blockSize;
		}
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
