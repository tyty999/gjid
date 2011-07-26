// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gif.h"
#include "image.h"

namespace gif {

//----------------------------------------------------------------------
// CTable; the LZW string table.
//----------------------------------------------------------------------

static void Error (const char* msg) __attribute__((noreturn));
static void Error (const char* msg)
    { throw runtime_error (msg); }

static inline void ReadShort (istream& is, uint16_t& v)
{
    is >> reinterpret_cast<uint8_t*>(&v)[0];
    is >> reinterpret_cast<uint8_t*>(&v)[1];
}

static inline void WriteShort (ostream& os, const uint16_t& v)
{
    os << reinterpret_cast<const uint8_t*>(&v)[0];
    os << reinterpret_cast<const uint8_t*>(&v)[1];
}

//----------------------------------------------------------------------

/// Default constructor.
CTable::CTable (void)
:_char ()
,_prev ()
,_first ()
,_nCodes (0)
,_resetCode (0)
,_endCode (0)
{
}

/// \brief Resets the table to the original code size of \p nRootBits.
///
/// Populates the table with (1 << (nRootBits - 1)) root entries plus
/// the reset and end-of-input codes. All other strings are erased.
/// This table supports codes up to 12 bits long, as in GIF standard.
///
void CTable::Reset (size_t nRootBits)
{
    const int n = 1 << (nRootBits - 1);
    _codeBits = nRootBits;
    _resetCode = n;
    _endCode = _resetCode + 1;
    _nCodes = _endCode + 1;
    if (_nCodes > c_MaxCodes)
	Error ("lzw table size is too large");
    fill (_char, chr_t(0));
    fill (_prev, c_Unused);
    fill (_first, c_Unused);
    for (code_t i = 0; i < _nCodes; ++ i)
	_char[i] = _first[i] = i;
}

/// Add a string base+chr to string table
CTable::code_t CTable::AddString (code_t base, chr_t c, bool bWriting)
{
    if (_nCodes >= c_MaxCodes || base == _nCodes)
	Error ("the compressed data is corrupt");
    _char [_nCodes] = c;
    _prev [_nCodes] = base;
    _first [_nCodes] = _first [base];
    if (++ _nCodes > BitMask(code_t,_codeBits) + bWriting)
	++ _codeBits;
    return (_nCodes - 1);
}

/// Writes string indexed by \p code into \p os.
void CTable::WriteString (code_t code, ostream& os) const
{
    chrvec_t buf;
    chrvec_t::iterator i = buf.begin();
    while (code < _nCodes && i != buf.end()) {
	*i++ = _char[code];
	code = _prev[code];
    }
    const size_t n (distance (buf.begin(), i));
    if (os.remaining() < n)
	os.verify_remaining ("lzw write", "gif", n);
    ostream::iterator io = os.ipos();
    for (uoff_t j = 0; j < n; ++j)
	*io++ = *--i;
    os.skip (n);
}

//----------------------------------------------------------------------
// CDecompressor; the decompression engine.
//----------------------------------------------------------------------

/// Default constructor.
CDecompressor::CDecompressor (void)
: t()
,_curByte (0)
,_bitsLeft (0)
,_blockSize (0)
{
}

/// Decompresses stream \p is into \p os.
void CDecompressor::Run (istream& is, ostream& os)
{
    if (is.remaining() < 2)
	return;
    uint8_t codeBits;
    is >> codeBits;
    t.Reset (++codeBits);

    CTable::code_t oldc = 0, c;
    while ((c = ReadCode (is)) != t.EndCode()) {
	if (c == t.ResetCode())
	    t.Reset (codeBits);
	else {
	    if (oldc != t.ResetCode() && !t.IsUnused(oldc)) {
		CTable::code_t k = t.IsUnused(c) ? oldc : c;
		t.AddString (oldc, t.FirstCharOf(k));
	    }
	    t.WriteString (c, os);
	}
	oldc = c;
    }
    // EOI code should be followed by the usual 0-length terminal block.
    is.skip (min (is.remaining(), _blockSize + 1U));
    _blockSize = 0;
}

/// Reads t.CodeBits bits from \p is.
CTable::code_t CDecompressor::ReadCode (istream& is)
{
    CTable::code_t c = 0;
    size_t bitsRead = 0;
    while (bitsRead < t.CodeBits()) {
	if (!_bitsLeft) {
	    if (!_blockSize) {
		is >> _blockSize;
		if (is.remaining() < size_t(_blockSize + !!_blockSize))
		    is.verify_remaining ("lzw read", "gif", _blockSize + !!_blockSize);
	    }
	    if (!_blockSize)
		return (t.EndCode());
	    is >> _curByte;
	    -- _blockSize;
	    _bitsLeft = 8;
	}
	size_t btr = min (_bitsLeft, t.CodeBits() - bitsRead);
	c |= ((_curByte & BitMask(uint16_t,btr)) << bitsRead);
	_curByte >>= btr;
	_bitsLeft -= btr;
	bitsRead += btr;
    }
    return (c);
}

//----------------------------------------------------------------------
// Blocks headers.
//----------------------------------------------------------------------

const char CFileHeader::s_GifSig[7] = "GIF89a";

//----------------------------------------------------------------------

/// Default constructor.
CFileHeader::CFileHeader (void)
:_width (0)
,_height (0)
,_resolution (0xFF)
,_background (0)
,_aspectRatio (0)
{
}

/// Reads the object from stream \p is.
void CFileHeader::read (istream& is)
{
    char sig [7];
    if (is.remaining() < 13)
	Error ("no GIF89 data found in the input stream");
    is.read (sig, 6);
    if (strncmp (sig, s_GifSig, 6))
	Error ("no GIF89 data found in the input stream");
    ReadShort (is, _width);
    ReadShort (is, _height);
    is >> _resolution >> _background >> _aspectRatio;
}

/// Writes the object to stream \p os.
void CFileHeader::write (ostream& os) const
{
    os.write (s_GifSig, 6);
    WriteShort (os, _width);
    WriteShort (os, _height);
    os << _resolution << _background << _aspectRatio;
}

//----------------------------------------------------------------------

/// Default constructor.
CImageHeader::CImageHeader (void)
:_leftBorder (0)
,_topBorder (0)
,_width (0)
,_height (0)
,_flags (0)
{
}

/// Reads the object from stream \p is.
void CImageHeader::read (istream& is)
{
    ReadShort (is, _leftBorder);
    ReadShort (is, _topBorder);
    ReadShort (is, _width);
    ReadShort (is, _height);
    is >> _flags;
}

/// Writes the object to stream \p os.
void CImageHeader::write (ostream& os) const
{
    WriteShort (os, _leftBorder);
    WriteShort (os, _topBorder);
    WriteShort (os, _width);
    WriteShort (os, _height);
    os << _flags;
}

//----------------------------------------------------------------------

/// Default constructor.
CGraphicsControl::CGraphicsControl (void)
:_delayTime (0)
,_flags (0)
,_transparentColor (0)
{
}

/// Read the object from stream \p is.
void CGraphicsControl::read (istream& is)
{
    uint8_t v;
    is >> v;
    if (v != 4)
	Error ("corrupt graphics control block in gif");
    is >> _flags;
    ReadShort (is, _delayTime);
    is >> _transparentColor;
    is >> v;
}

/// Write the object to stream \p os.
void CGraphicsControl::write (ostream& os) const
{
    uint8_t blockSize = 4;
    os << blockSize;
    os << _flags;
    WriteShort (os, _delayTime);
    os << _transparentColor;
    os << GIF_ZERO_BLOCK;
}

//----------------------------------------------------------------------

/// Reads the object from stream \p is.
void CComment::read (istream& is)
{
    _s.clear();
    for (;;) {
	uint8_t bs;
	is >> bs;
	if (is.remaining() < bs + 1U)
	    break;
	_s.append (is.ipos(), bs);
	is.skip (bs);
    }
}

/// Writes the object to stream \p os.
void CComment::write (ostream& os) const
{
    foreach (string::const_iterator, i, _s) {
	uint8_t bs = min (distance (i, _s.end()), GIF_MAX_BLOCK_SIZE);
	os << bs;
	os.write (i, bs);
	i += bs;
    }
    os << GIF_ZERO_BLOCK;
}

/// Returns the size of the written object.
size_t CComment::stream_size (void) const
{
    const size_t nBlocks = (_s.size() + GIF_MAX_BLOCK_SIZE - 1) / GIF_MAX_BLOCK_SIZE;
    return (_s.size() + nBlocks + stream_size_of(GIF_ZERO_BLOCK));
}

//----------------------------------------------------------------------

} // namespace gif
