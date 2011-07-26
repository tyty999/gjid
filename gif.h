// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// Thanks to Gunter Born and his "The File Format Handbook" for the
// structure of the headers and the data blocks. Also, big thanks to
// John Barkaus for his '89 USENET post explaining the LZW algorithm.

#pragma once
#include "config.h"

class CImage;

namespace gif {

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
    enum {
	c_MaxCodeBits = 12,
	c_MaxCodes = 1 << c_MaxCodeBits,
	c_Unused = c_MaxCodes
    };
    typedef tuple<c_MaxCodes,code_t>	ptrvec_t;
    typedef tuple<c_MaxCodes,chr_t>	chrvec_t;
    typedef chrvec_t::const_iterator	chrciter_t;
public:
			CTable (void);
    void		Reset (size_t nRootBits);
    code_t		AddString (code_t base, chr_t c, bool bWriting = false);
    void		WriteString (code_t code, ostream& os) const;
    inline chr_t	FirstCharOf (code_t code) const	{ return (_char [_first [code]]); }
    inline bool		IsUnused (code_t code) const	{ return (code >= _nCodes); }
    inline code_t	ResetCode (void) const		{ return (_resetCode); }
    inline code_t	EndCode (void) const		{ return (_endCode); }
    inline uint8_t	CodeBits (void) const		{ return (_codeBits); }
private:
    chrvec_t		_char;		///< Last characters of each string.
    ptrvec_t		_prev;		///< Pointers to the previous character.
    ptrvec_t		_first;	///< Pointers to the first character of the string.
    code_t		_nCodes;	///< Last used string index.
    code_t		_resetCode;	///< "Reset Table" code.
    code_t		_endCode;	///< "End Of Input" code.
    uint16_t		_codeBits;	///< Bits required to address all active codes.
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
    uint8_t		_curByte;	///< Last byte read from the input stream.
    uint8_t		_bitsLeft;	///< Unread bits left in _curByte.
    uint8_t		_blockSize;	///< Remaining bytes in the current block.
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
    inline bool		HasGlobalCmap (void) const	{ return (_resolution & 0x80); }
    inline void		SetGlobalCmap (bool v = true)	{ if (v) _resolution |= 0x80; else _resolution &= 0x7F; }
    inline bool		SortedCmap (void) const		{ return (_resolution & 0x08); }
    inline void		SetSortedCmap (bool v = true)	{ if (v) _resolution |= 0x08; else _resolution &= 0xF7; }
    inline size_t	BitsPerPixel (void) const	{ return ((_resolution & 0x07) + 1); }
    inline void		SetBitsPerPixel (size_t n)	{ _resolution &= 0xF8; _resolution |= (n - 1) & 0x07; }
    inline size_t	BitsPerRay (void) const		{ return (((_resolution >> 4) & 0x07) + 1); }
    inline void		SetBitsPerRay (size_t n)	{ _resolution &= 0x8F; _resolution |= ((n - 1) & 0x07) << 4; }
public:
    uint16_t		_width;		///< Width of the "screen", the total area for all images in this file.
    uint16_t		_height;		///< Height of the "screen".
    uint8_t		_resolution;		///< Miscellaneous flags.
    uint8_t		_background;		///< Background color index.
    uint8_t		_aspectRatio;		///< Image aspect ratio.
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
    inline bool		HasLocalCmap (void) const	{ return (_flags & 0x80); }
    inline void		SetLocalCmap (bool v = true)	{ if (v) _flags |= 0x80; else _flags &= 0x7F; }
    inline bool		Interlaced (void) const		{ return (_flags & 0x40); }
    inline void		SetInterlaced (bool v = true)	{ if (v) _flags |= 0x40; else _flags &= 0xBF; }
    inline bool		SortedCmap (void) const		{ return (_flags & 0x20); }
    inline void		SetSortedCmap (bool v = true)	{ if (v) _flags |= 0x20; else _flags &= 0xDF; }
    inline size_t	BitsPerPixel (void) const	{ return ((_flags & 0x07) + 1); }
    inline void		SetBitsPerPixel (size_t n)	{ _flags &= 0xF8; _flags |= (n - 1) & 0x07; }
public:
    uint16_t		_leftBorder;	///< Pixel width of the left/right border.
    uint16_t		_topBorder;	///< Width of the top/bottom border.
    uint16_t		_width;	///< Width of the image.
    uint16_t		_height;	///< Height of the image.
    uint8_t		_flags;	///< Various flags.
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
    inline bool		HasTransparent (void) const	{ return (_flags & 1); }
    inline void		SetTransparent (uint8_t c)	{ _flags |= 1; _transparentColor = c; }
    inline bool		RequiresInput (void) const	{ return (_flags & 2); }
    inline void		SetRequiresInput (bool v)	{ if (v) _flags |= 2; else _flags &= ~2; }
    inline EDisposal	Disposal (void) const		{ return (EDisposal (_flags >> 2)); }
    inline void		SetDisposal (EDisposal d)	{ _flags &= 0x1A; _flags |= d << 2; }
public:
    uint16_t		_delayTime;		///< Delay time before action (0.01s)
    uint8_t		_flags;		///< Various flags.
    uint8_t		_transparentColor;	///< Transparent color index.
};

/// \class CComment gif fbgl/gif.h
///
/// \brief Comment block.
///
class CComment {
public:
			CComment (const string& s) : _s (s) {}
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
private:
    string		_s;	///< Comment text.
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
