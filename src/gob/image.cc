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
  m_Size (0, 0),
  m_Flags (0)
{
}

/// Resize the image to the given dimensions.
void CImage::Resize (Size2d sz)
{
    m_Pixels.resize (sz[0] * sz[1]);
    m_Size = sz;
}

/// Attaches to pixels at \p l of size \p sz.
void CImage::link (memlink l, Size2d sz)
{
    m_Pixels.link ((pixel_t*) l.begin(), (pixel_t*) l.end());
    m_Size = sz;
    assert (m_Pixels.size() == Width() * Height());
}

/// Detaches from the current pixel data.
void CImage::unlink (void)
{
    m_Pixels.unlink();
    m_Size = Size2d();
}

/// Returns the colordepth necessary to display this image.
size_t CImage::BitsPerPixel (void) const
{
    size_t bpp = 1;
    while (m_Palette.size() > (1U << bpp))
	++ bpp;
    return (bpp);
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

void CImage::WriteGifColormap (ostream& os) const
{
    const size_t nCols (1 << BitsPerPixel());
    for (uoff_t i = 0; i < nCols; ++ i) {
	ray_t r = 0, g = 0, b = 0;
	if (i < m_Palette.size())
	    unRGB (m_Palette[i], r, g, b);
	os << r << g << b;
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

/// Writes the object to stream \p os.
void CImage::write (ostream& os) const
{
    using namespace gif;
    CFileHeader fh;
    fh.m_Width = Width();
    fh.m_Height = Height();
    fh.SetGlobalCmap (true);
    fh.SetBitsPerPixel (BitsPerPixel());
    fh.SetSortedCmap (Flag (f_SortedPalette));
    os << fh;
    WriteGifColormap (os);

    if (Flag (f_Transparent)) {
	CGraphicsControl gch;
	gch.SetTransparent (0);
	os << GIF_EXT_BLOCK_SIG << GIF_EXT_GC_SIG << gch;
    }

    CImageHeader ih;
    ih.m_Width = Width();
    ih.m_Height = Width();
    os << GIF_IMAGE_BLOCK_SIG << ih;
    CCompressor c;
    istream is (begin(), Width() * Height());
    c.SetCodeSize (fh.BitsPerPixel());
    c.Run (is, os);

    os << GIF_END_OF_DATA_SIG;
}

/// Returns the size of the written object.
size_t CImage::stream_size (void) const
{
    size_t s = stream_size_of (gif::CFileHeader());
    s += 3 * (1 << BitsPerPixel());	// colormap
    if (Flag (f_Transparent)) {
	s += stream_size_of (GIF_EXT_BLOCK_SIG) + stream_size_of (GIF_EXT_GC_SIG);
	s += stream_size_of (gif::CGraphicsControl());
    }
    s += stream_size_of (GIF_IMAGE_BLOCK_SIG);
    s += stream_size_of (gif::CImageHeader());
    gif::CCompressor c;
    istream is (begin(), Width() * Height());
    c.SetCodeSize (BitsPerPixel());
    s += c.EstimateSize (is);
    s += stream_size_of (GIF_END_OF_DATA_SIG);
    return (s);
}

} // namespace fbgl

