// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// strt.h
//

#ifndef STRT_H_2D10D5005E82A1AC666F7ED976236431
#define STRT_H_2D10D5005E82A1AC666F7ED976236431

#include <ustl.h>

namespace fbgl {
using namespace ustl;

/// \class CStringTable strt.h fbgl.h
///
/// \brief An array of strings.
/// Efficiently stores many strings in a single memory block. Reading and
/// appending are O(1) (both slower than a direct access though), removing
/// is O(n). Size overhead is minimal.
///
class CStringTable {
public:
    typedef string		value_type;
    typedef const value_type&	const_reference;
public:
				CStringTable (void);
    const string		at (uoff_t i) const;
    inline const string		operator[] (uoff_t i) const	{ return (at(i)); }
    inline size_t		size (void) const		{ return (m_Index.size() - 1); }
    inline size_t		capacity (void) const		{ return (m_Index.capacity() - 1); }
    inline bool			empty (void) const		{ return (!size()); }
    void			erase (uoff_t i);
    void			push_back (const string& s);
    void			clear (void);
    void			read (istream& is);
    void			write (ostream& os) const;
    size_t			stream_size (void) const;
private:
    typedef vector<uoff_t>	idxvec_t;
private:
    size_t			ComputeCodeSize (void) const;
private:
    idxvec_t			m_Index;	///< Index into \p m_Data.
    string			m_Data;		///< String data.
};

} // namespace fbgl

STD_STREAMABLE (fbgl::CStringTable)

#endif

