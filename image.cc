// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "image.h"
#include "gif.h"

//----------------------------------------------------------------------
// Memory management.
//----------------------------------------------------------------------

/// Default constructor.
CImage::CImage (void)
:_pixels()
,_palette()
,_width (0)
,_height (0)
{
}

/// Default constructor.
CImage::CImage (dim_t w, dim_t h, const color_t* p)
{
    Resize (w, h);
    if (p)
	copy_n (p, w * h, _pixels.begin());
}

/// Resize the image to the given dimensions.
void CImage::Resize (dim_t w, dim_t h)
{
    _pixels.resize (w * h);
    _width = w;
    _height = h;
}

//----------------------------------------------------------------------
// Palette operations.
//----------------------------------------------------------------------

/// Maps the image colors onto \p pal. Call after reading, before drawing.
void CImage::MergePaletteInto (CPalette& pal)
{
    assert (!_palette.empty() && "This image doesn't have a palette to merge");
    foreach (CPalette::iterator, i, _palette)
	setRayA (*i, pal.AllocColor(*i));
    foreach (pixvec_t::iterator, i, _pixels)
	*i = getRayA (_palette [min (*i, _palette.size()-1)]);
}

//----------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------

/// Reads the colormap from a GIF file.
void CImage::ReadGifColormap (istream& is, size_t bpp)
{
    _palette.resize (1 << bpp);
    foreach (CPalette::iterator, i, _palette) {
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
	    if (ih._width > 16000 || ih._height > 16000)
		throw runtime_error ("invalid image size");
	    Resize (ih._width, ih._height);
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
