// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// strt.cc
//

#include "strt.h"

namespace fbgl {

/// Default constructor.
CStringTable::CStringTable (void)
: m_Index (),
  m_Data ()
{
    Clear();
}

/// Returns the string number \p i.
string CStringTable::at (uoff_t i) const
{
    string v;
    string::const_iterator first, last;
    first = m_Data.begin() + m_Index[i];
    last = m_Data.begin() + m_Index[i + 1];
    v.link (first, last);
    return (v);
}

/// Clears the string table.
void CStringTable::Clear (void)
{
    m_Index.clear();
    m_Index.push_back (m_Data.size());
    m_Data.clear();
}

/// Removes string \p i from the table.
void CStringTable::Remove (uoff_t i)
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
void CStringTable::Add (const string& s)
{
    m_Data.append (s, s.size() + 1);
    m_Index.push_back (m_Data.size());
}

/// Reads the object from stream \p is.
void CStringTable::read (istream& is)
{
    is >> m_Index;
    is >> m_Data;
    is.align();
}

/// Writes the object to stream \p os.
void CStringTable::write (ostream& os) const
{
    os << m_Index;
    os << m_Data;
    os.align();
}

/// Returns the size of the written object.
size_t CStringTable::stream_size (void) const
{
    return (Align (stream_size_of (m_Index) + stream_size_of (m_Data)));
}

} // namespace fbgl

