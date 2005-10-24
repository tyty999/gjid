// targa.cc
//
// TARGA format information
//

#include "targa.h"

//----------------------------------------------------------------------

const char CTargaHeader::s_IDField[6] = "FBGLI";

//----------------------------------------------------------------------

/// Default constructor.
CTargaHeader::CTargaHeader (void)
: m_ImgIDLen (VectorSize(s_IDField)),
  m_bHasCMap (false),
  m_Type (timg_CMapUncompressed),
  m_CMapBits (32),
  m_CMapFirst (0),
  m_CMapLength (0),
  m_XOrigin (0),
  m_YOrigin (0),
  m_Width (0),
  m_Height (0),
  m_Depth (8),
  m_Flags (1 << f_UpperOrigin)
{
}

/// Reads the object from stream \p is.
void CTargaHeader::read (istream& is)
{
    is >> m_ImgIDLen >> m_bHasCMap >> m_Type;
    is.read (&m_CMapFirst, 2);
    is.read (&m_CMapLength, 2);
    is >> m_CMapBits;
    is >> m_XOrigin >> m_YOrigin >> m_Width >> m_Height;
    is >> m_Depth >> m_Flags;
    is.skip (m_ImgIDLen);
}

/// Writes the object into stream \p os.
void CTargaHeader::write (ostream& os) const
{
    os << m_ImgIDLen << m_bHasCMap << m_Type;
    os.write (&m_CMapFirst, 2);
    os.write (&m_CMapLength, 2);
    os << m_CMapBits;
    os << m_XOrigin << m_YOrigin << m_Width << m_Height;
    os << m_Depth << m_Flags;
    os.write (s_IDField, VectorSize(s_IDField));
}

/// Returns the size of the written object.
size_t CTargaHeader::stream_size (void) const
{
    return (stream_size_of (m_ImgIDLen) +
	    stream_size_of (m_bHasCMap) +
	    stream_size_of (m_Type) + 4 +
	    stream_size_of (m_CMapBits) +
	    stream_size_of (m_XOrigin) +
	    stream_size_of (m_YOrigin) +
	    stream_size_of (m_Width) +
	    stream_size_of (m_Height) +
	    stream_size_of (m_Depth) +
	    stream_size_of (m_Flags) +
	    VectorSize (s_IDField));
}

/// Fills in values based on \p pal.
void CTargaHeader::SetPalette (const CPalette& pal)
{
    m_bHasCMap = !pal.empty();
    m_CMapLength = pal.size();
}

/// Fills in values based on \p img.
void CTargaHeader::SetImage (const Icon& img)
{
    m_Width = img.Width();
    m_Height = img.Height();
}

