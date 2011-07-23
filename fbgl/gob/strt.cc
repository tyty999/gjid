// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "strt.h"
#include "gif.h"

namespace fbgl {

/// Default constructor.
CStringTable::CStringTable (void)
: m_Index (),
  m_Data ()
{
    clear();
}

/// Returns the string number \p i.
const string CStringTable::at (uoff_t i) const
{
    string v;
    string::const_iterator first, last;
    first = m_Data.begin() + m_Index[i];
    last = m_Data.begin() + m_Index[i + 1] - 1;
    v.link (first, last);
    return (v);
}

/// Clears the string table.
void CStringTable::clear (void)
{
    m_Index.clear();
    m_Index.push_back (m_Data.size());
    m_Data.clear();
}

/// Removes string \p i from the table.
void CStringTable::erase (uoff_t i)
{
    string::iterator first, last;
    first = m_Data.begin() + m_Index[i];
    last = m_Data.begin() + m_Index[i + 1];
    size_t nToErase = distance (first, last);
    for (uoff_t j = i + 1; j < m_Index.size(); ++ j)
	m_Index[j] -= nToErase;
    m_Index.erase (m_Index.begin() + i);
    m_Data.erase (first, last);
}

/// Adds string \p s to the table.
void CStringTable::push_back (const string& s)
{
    m_Data.append (s.begin(), s.size());
    m_Data.push_back ('\0');
    m_Index.push_back (m_Data.size());
}

static const char g_StrtSig[4] = {'S','T','R','T'};

/// Determines the maximum character width.
size_t CStringTable::ComputeCodeSize (void) const
{
    size_t cs = 1;
    foreach (string::const_iterator, i, m_Data)
	while (*i >= (1 << cs))
	    ++ cs;
    return (cs);
}

/// Reads the object from stream \p is.
void CStringTable::read (istream& is)
{
    char sig [VectorSize(g_StrtSig)];
    uint32_t nStrings, nChars, flags;
    is.read (sig, VectorSize(sig));
    if (strncmp (sig, g_StrtSig, VectorSize(g_StrtSig)))
	throw runtime_error ("no string table found in the input stream");
    is >> nStrings >> nChars >> flags;

    m_Data.resize (nChars);
    ostream os (m_Data.begin(), m_Data.size());
    gif::CDecompressor d;
    d.Run (is, os);
    if (os.remaining())
	throw runtime_error ("string table data is corrupt");
    is.align(4);

    m_Index.clear();
    m_Index.reserve (nStrings);
    m_Index.push_back (0);
    for (uoff_t i = 0; i < m_Data.size(); ++ i)
	if (!m_Data[i])
	    m_Index.push_back (i + 1);
}

/// Writes the object to stream \p os.
void CStringTable::write (ostream& os) const
{
    uint32_t flags = 1;
    os.write (g_StrtSig, VectorSize(g_StrtSig));
    os << uint32_t(m_Index.size()) << uint32_t(m_Data.size()) << flags;
    istream is (m_Data.begin(), m_Data.size());
    gif::CCompressor c;
    c.SetCodeSize (ComputeCodeSize());
    c.Run (is, os);
    os.align(4);
}

/// Returns the size of the written object.
size_t CStringTable::stream_size (void) const
{
    const size_t headerSize (VectorSize(g_StrtSig) +
	    stream_size_of (uint32_t(m_Index.size())) +
	    stream_size_of (uint32_t(m_Data.size())) +
	    stream_size_of (uint32_t(1)));
    istream is (m_Data.begin(), m_Data.size());
    gif::CCompressor c;
    c.SetCodeSize (ComputeCodeSize());
    const uint32_t dataSize = c.EstimateSize (is);
    return (headerSize + Align (dataSize));
}

} // namespace fbgl
