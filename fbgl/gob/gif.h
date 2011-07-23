// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// Thanks to Gunter Born and his "The File Format Handbook" for the
// structure of the headers and the data blocks. Also, big thanks to
// John Barkaus for his '89 USENET post explaining the LZW algorithm.

#include <ustl.h>

class CImage;

namespace gif {
using namespace ustl;

//----------------------------------------------------------------------
// LZW string table
//----------------------------------------------------------------------

/// \class CTable gif.h fbgl/gif.h
///
/// \brief Implements the LZW string table.
///
/// Maintains a list of character sequences in delta-list form. Because each
/// new string is equal to its base plus one character, the strings are stored
/// as that character and the base index. "Root" strings have no base and are
/// one character long. See Reset function documentation for the initial state.
///
class CTable {
public:
    typedef uint16_t			code_t;
    typedef uint8_t			chr_t;
    static const code_t	c_MaxCodeBits = 12;
    static const code_t	c_MaxCodes = 1 << c_MaxCodeBits;
    static const code_t	c_Unused = c_MaxCodes;
    typedef tuple<c_MaxCodes,code_t>	ptrvec_t;
    typedef tuple<c_MaxCodes,chr_t>	chrvec_t;
    typedef chrvec_t::const_iterator	chrciter_t;
public:
			CTable (void);
    void		Reset (size_t nRootBits);
    code_t		AddString (code_t base, chr_t c, bool bWriting = false);
    code_t		Find (chrciter_t first, chrciter_t last) const;
    void		WriteString (code_t code, ostream& os) const;
    inline chr_t	FirstCharOf (code_t code) const	{ return (m_Char [m_First [code]]); }
    inline bool		IsUnused (code_t code) const	{ return (code >= m_nCodes); }
    inline code_t	BaseOf (code_t code) const	{ return (m_Prev [code]); }
    inline bool		IsRoot (code_t code) const	{ return (BaseOf(code) == c_Unused); }
    inline code_t	ResetCode (void) const		{ return (m_ResetCode); }
    inline code_t	EndCode (void) const		{ return (m_EndCode); }
    inline code_t	LastUsedCode (void) const	{ return (m_nCodes - 1); }
    inline uint8_t	CodeBits (void) const		{ return (m_CodeBits); }
private:
    inline bool		CodeIsString (code_t c, const chrciter_t first, chrciter_t last) const;
private:
    chrvec_t		m_Char;		///< Last characters of each string.
    ptrvec_t		m_Prev;		///< Pointers to the previous character.
    ptrvec_t		m_First;	///< Pointers to the first character of the string.
    code_t		m_nCodes;	///< Last used string index.
    code_t		m_ResetCode;	///< "Reset Table" code.
    code_t		m_EndCode;	///< "End Of Input" code.
    uint16_t		m_CodeBits;	///< Bits required to address all active codes.
};

//----------------------------------------------------------------------
// Decompressor.
//----------------------------------------------------------------------

/// \class CDecompressor gif.h fbgl/gif.h
///
/// \brief Decompresses GIF pixel data.
///
/// Implements the GIF LZW decompression algorithm, as described by John
/// Barkaus in a USENET post to comp.graphics in 1989.
///
class CDecompressor {
public:
			CDecompressor (void);
    void		Run (istream& is, ostream& os);
private:
    CTable::code_t	ReadCode (istream& is);
private:
    CTable		t;		///< The decompression string table.
    uint8_t		m_CurByte;	///< Last byte read from the input stream.
    uint8_t		m_BitsLeft;	///< Unread bits left in m_CurByte.
    uint8_t		m_BlockSize;	///< Remaining bytes in the current block.
};

//----------------------------------------------------------------------
// Compressor.
//----------------------------------------------------------------------

/// \class CCompressor gif.h fbgl/gif.h
///
/// \brief Compresses pixel data with the GIF LZW algorithm.
///
class CCompressor {
public:
			CCompressor (void);
    void		Run (istream& is, ostream& os);
    size_t		EstimateSize (istream& is);
    void		SetCodeSize (size_t n);
private:
    void		WriteCode (ostream& os, CTable::code_t c);
    inline void		FlushCurByte (ostream& os);
    void		StartNextBlock (ostream& os);
private:
    CTable		t;
    uint8_t		m_CodeSize;
    uint8_t		m_BlockSize;
    uint8_t		m_CurByte;
    uint8_t		m_BitsUsed;
};

//----------------------------------------------------------------------
// Block headers.
//----------------------------------------------------------------------

#define GIF_EXT_BLOCK_SIG	uint8_t('!')
#define GIF_IMAGE_BLOCK_SIG	uint8_t(',')
#define GIF_END_OF_DATA_SIG	uint8_t(';')
#define GIF_MAX_BLOCK_SIZE	uint8_t(0xFE)
#define GIF_ZERO_BLOCK		uint8_t(0)

#define GIF_EXT_GC_SIG		uint8_t(0xF9)
#define GIF_EXT_COMMENT_SIG	uint8_t(0xFE)

//----------------------------------------------------------------------

/// \class CFileHeader gif.h "gif.h"
///
/// \brief First header of the GIF file.
///
class CFileHeader {
public:
			CFileHeader (void);
    void		read (istream& is);
    void		write (ostream& os) const;
    inline size_t	stream_size (void) const	{ return (13); }
    inline bool		HasGlobalCmap (void) const	{ return (m_Resolution & 0x80); }
    inline void		SetGlobalCmap (bool v = true)	{ if (v) m_Resolution |= 0x80; else m_Resolution &= 0x7F; }
    inline bool		SortedCmap (void) const		{ return (m_Resolution & 0x08); }
    inline void		SetSortedCmap (bool v = true)	{ if (v) m_Resolution |= 0x08; else m_Resolution &= 0xF7; }
    inline size_t	BitsPerPixel (void) const	{ return ((m_Resolution & 0x07) + 1); }
    inline void		SetBitsPerPixel (size_t n)	{ m_Resolution &= 0xF8; m_Resolution |= (n - 1) & 0x07; }
    inline size_t	BitsPerRay (void) const		{ return (((m_Resolution >> 4) & 0x07) + 1); }
    inline void		SetBitsPerRay (size_t n)	{ m_Resolution &= 0x8F; m_Resolution |= ((n - 1) & 0x07) << 4; }
public:
    uint16_t		m_Width;		///< Width of the "screen", the total area for all images in this file.
    uint16_t		m_Height;		///< Height of the "screen".
    uint8_t		m_Resolution;		///< Miscellaneous flags.
    uint8_t		m_Background;		///< Background color index.
    uint8_t		m_AspectRatio;		///< Image aspect ratio.
    static const char	s_GifSig [7];		///< "GIF89a" signature string.
};

//----------------------------------------------------------------------

/// \class CImageHeader gif.h fbgl/gif.h
///
/// \brief Header preceeding the compressed image data.
///
class CImageHeader {
public:
			CImageHeader (void);
    void		read (istream& is);
    void		write (ostream& os) const;
    inline size_t	stream_size (void) const	{ return (9); }
    inline bool		HasLocalCmap (void) const	{ return (m_Flags & 0x80); }
    inline void		SetLocalCmap (bool v = true)	{ if (v) m_Flags |= 0x80; else m_Flags &= 0x7F; }
    inline bool		Interlaced (void) const		{ return (m_Flags & 0x40); }
    inline void		SetInterlaced (bool v = true)	{ if (v) m_Flags |= 0x40; else m_Flags &= 0xBF; }
    inline bool		SortedCmap (void) const		{ return (m_Flags & 0x20); }
    inline void		SetSortedCmap (bool v = true)	{ if (v) m_Flags |= 0x20; else m_Flags &= 0xDF; }
    inline size_t	BitsPerPixel (void) const	{ return ((m_Flags & 0x07) + 1); }
    inline void		SetBitsPerPixel (size_t n)	{ m_Flags &= 0xF8; m_Flags |= (n - 1) & 0x07; }
public:
    uint16_t		m_LeftBorder;	///< Pixel width of the left/right border.
    uint16_t		m_TopBorder;	///< Width of the top/bottom border.
    uint16_t		m_Width;	///< Width of the image.
    uint16_t		m_Height;	///< Height of the image.
    uint8_t		m_Flags;	///< Various flags.
};

//----------------------------------------------------------------------

/// \class CGraphicsControl gif fbgl/gif.h
///
/// \brief Defines the format of the graphics control extension block.
///
class CGraphicsControl {
public:
    /// Describes how the decoder should dispose of the image.
    enum EDisposal {
	dis_None,		///< Any way it wants
	dis_Leave,		///< Leave it where it is
	dis_RestoreBack,	///< Overwrite with background
	dis_RestorePrev		///< OVerwrite with what was there before
    };
public:
			CGraphicsControl (void);
    void		read (istream& is);
    void		write (ostream& os) const;
    inline size_t	stream_size (void) const	{ return (6); }
    inline bool		HasTransparent (void) const	{ return (m_Flags & 1); }
    inline void		SetTransparent (uint8_t c)	{ m_Flags |= 1; m_TransparentColor = c; }
    inline bool		RequiresInput (void) const	{ return (m_Flags & 2); }
    inline void		SetRequiresInput (bool v)	{ if (v) m_Flags |= 2; else m_Flags &= ~2; }
    inline EDisposal	Disposal (void) const		{ return (EDisposal (m_Flags >> 2)); }
    inline void		SetDisposal (EDisposal d)	{ m_Flags &= 0x1A; m_Flags |= d << 2; }
public:
    uint16_t		m_DelayTime;		///< Delay time before action (0.01s)
    uint8_t		m_Flags;		///< Various flags.
    uint8_t		m_TransparentColor;	///< Transparent color index.
};

/// \class CComment gif fbgl/gif.h
///
/// \brief Comment block.
///
class CComment {
public:
			CComment (const string& s) : m_s (s) {}
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
private:
    string		m_s;	///< Comment text.
};

//----------------------------------------------------------------------

} // namespace gif

ALIGNOF(gif::CFileHeader, 1)
ALIGNOF(gif::CImageHeader, 1)
ALIGNOF(gif::CGraphicsControl, 1)
ALIGNOF(gif::CComment, 1)
STD_STREAMABLE (gif::CFileHeader)
STD_STREAMABLE (gif::CImageHeader)
STD_STREAMABLE (gif::CGraphicsControl)
STD_STREAMABLE (gif::CComment)
