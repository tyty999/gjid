// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "image.h"
#include "gif.h"

//----------------------------------------------------------------------
// Memory management.
//----------------------------------------------------------------------

/// Default constructor.
CImage::CImage (void)
: m_Pixels (),
  m_Palette (),
  m_Size (0, 0),
  m_Flags (0)
{
}

/// Default destructor.
CImage::~CImage (void)
{
}

/// Resize the image to the given dimensions.
void CImage::Resize (const Size2d& sz)
{
    m_Pixels.resize (sz[0] * sz[1]);
    m_Size = sz;
}

/// Attaches to pixels at \p l of size \p sz.
void CImage::link (const memlink& l, const Size2d& sz)
{
    m_Pixels.link ((pixel_t*) l.begin(), (pixel_t*) l.end());
    m_Size = sz;
    assert (m_Pixels.size() == size_t(Width() * Height()));
}

/// Detaches from the current pixel data.
void CImage::unlink (void)
{
    m_Pixels.unlink();
    m_Size = Size2d();
}

//----------------------------------------------------------------------
// Palette operations.
//----------------------------------------------------------------------

/// Puts \p c into the palette and returns its index.
color_t CImage::AllocColor (colordef_t c)
{
    assert (!Flag (f_MergedPalette) && "You can't add new colors to a merged image. Call NormalizePalette to make it writable.");
    return (m_Palette.AllocColor (c));
}

/// Returns the colordepth necessary to display this image.
size_t CImage::BitsPerPixel (void) const
{
    assert (!Flag (f_MergedPalette) && "Color depth can not be determined while in merged state. Call NormalizePalette first.");
    size_t bpp = 1;
    while (m_Palette.size() > (1U << bpp))
	++ bpp;
    #ifndef NDEBUG
	foreach (pixvec_t::const_iterator, i, m_Pixels)
	    assert (*i < (1U << bpp) && "You have been writing to the image data without writing new color definitions to the palette. Because of this, some colors can't be found in the palette. Writing this image will produce an unreadable file which may crash badly written readers.");
    #endif
    return (bpp);
}

/// Maps the image colors onto \p pal. Call after reading, before drawing.
void CImage::MergePaletteInto (CPalette& pal)
{
    assert (!m_Palette.empty() && "This image doesn't have a palette to merge");
    SetFlag (f_SortedPalette, false);
    foreach (CPalette::iterator, i, m_Palette)
	setRayA (*i, pal.AllocColor(*i));
    SetFlag (f_MergedPalette);
    foreach (pixvec_t::iterator, i, m_Pixels)
	*i = getRayA (m_Palette [min (*i, m_Palette.size()-1)]);
}

/// Reduces the colormap to minimum. Call before writing.
void CImage::NormalizePalette (void)
{
    if (Flag (f_MergedPalette)) {
	foreach (pixvec_t::iterator, i, m_Pixels)
	    for (uoff_t j = 0; j < m_Palette.size(); ++ j)
		if (*i == getRayA (m_Palette[j]))
		    *i = j;
	foreach (CPalette::iterator, i, m_Palette)
	    setRayA (*i, 0U);
	SetFlag (f_MergedPalette, false);
    }

    CPalette usedpal (m_Palette.size());
    iota (usedpal.begin(), usedpal.end(), 0);
    foreach (pixvec_t::const_iterator, i, m_Pixels)
	usedpal[*i] += 0x0100;	// Count color usage
    sort (usedpal);

    CPalette::iterator firstUsed = usedpal.begin();
    while (firstUsed != usedpal.end() && !(*firstUsed >> 8))
	++ firstUsed;
    usedpal.erase (usedpal.begin(), firstUsed);
    reverse (usedpal);

    assert ((!m_Palette.empty() || m_Pixels.empty()) && "This image has no colormap, so it is impossible to determine the pixel values!");
    foreach (pixvec_t::iterator, i, m_Pixels) {
	uoff_t j = 0;
	while (j < usedpal.size() && *i != (usedpal[j] & 0xFF)) ++j;
	assert (j < usedpal.size() && "This image contains pixel values not in the colormap.");
	*i = min (j, usedpal.size() - 1);
    }
    for (uoff_t i = 0; i < usedpal.size(); ++ i)
	usedpal[i] = m_Palette [usedpal[i] & 0xFF];
    m_Palette = usedpal;
    SetFlag (f_SortedPalette);
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
    m_Flags = 0;

    using namespace gif;
    CFileHeader fh;
    if (is.remaining() < stream_size_of(fh))
	return;	// empty file; not really bad.
    is >> fh;
    if (fh.HasGlobalCmap()) {
	SetFlag (f_SortedPalette, fh.SortedCmap());
	ReadGifColormap (is, fh.BitsPerPixel());
    }

    uint8_t c = 0;
    while (is.remaining()) {
	is >> c;
	if (c == GIF_EXT_BLOCK_SIG && is.remaining() >= 2) {
	    uint8_t extCode, blockSize = 0;
	    is >> extCode;
	    if (extCode == GIF_EXT_GC_SIG) {
		CGraphicsControl gch;
		is >> gch;
		SetFlag (f_Transparent, gch.HasTransparent());
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
	    Resize (Size2d (ih.m_Width, ih.m_Height));
	    if (ih.HasLocalCmap()) {
		SetFlag (f_SortedPalette, ih.SortedCmap());
		ReadGifColormap (is, ih.BitsPerPixel());
	    }
	    ostream os (begin(), Width() * Height());
	    CDecompressor d;
	    d.Run (is, os);
	} else if (c == GIF_END_OF_DATA_SIG)
	    break;
	else if (c)
	    throw runtime_error ("unrecognized block type found in the gif file");
    }
}
