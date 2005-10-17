// pal.h
//

#ifndef PAL_H_1AAA82416DDBAF8A56F30C956E52D7CE
#define PAL_H_1AAA82416DDBAF8A56F30C956E52D7CE

#include <ustl.h>

namespace fbgl {
using namespace ustl;

typedef uint8_t		ray_t;
typedef uint8_t		color_t;

/// \class CPalette pal.h fbgl.h
///
/// \brief Color palette for 8-bit pseudocolor modes.
///
class CPalette {
public:
    typedef ray_t			value_type;
    typedef tuple<256*3,value_type>	palvec_t;
    typedef palvec_t::iterator		iterator;
    typedef palvec_t::const_iterator	const_iterator;
    typedef size_t			size_type;
public:
    inline			CPalette (void)			: m_Data() { }
    inline void			read (istream& is)		{ is >> m_Data; }
    inline void			write (ostream& os) const	{ os << m_Data; }
    inline size_t		stream_size (void) const	{ return (stream_size_of (m_Data)); }
    inline const_iterator	begin (void) const		{ return (m_Data.begin()); }
    inline iterator		begin (void)			{ return (m_Data.begin()); }
    inline const_iterator	end (void) const		{ return (m_Data.end()); }
    inline iterator		end (void)			{ return (m_Data.end()); }
    inline size_type		size (void) const		{ return (256); }
    inline size_type		capacity (void) const		{ return (256); }
    inline const_iterator	at (color_t i) const		{ return (&m_Data [i * 3]); }
    inline iterator		at (color_t i)			{ return (&m_Data [i * 3]); }
    inline const_iterator	operator[] (color_t i) const	{ return (at(i)); }
    inline iterator		operator[] (color_t i)		{ return (at(i)); }
    inline void			Set (color_t i, ray_t r, ray_t g, ray_t b)		{ iterator ii (at(i)); ii[0] = r; ii[1] = g; ii[2] = b; }
    void			Get (color_t i, ray_t& r, ray_t& g, ray_t& b) const	{ const_iterator ii (at(i)); r = ii[0]; g = ii[1]; b = ii[2]; }
private:
    palvec_t		m_Data;
};

} // namespace fbgl

STD_STREAMABLE (fbgl::CPalette)

#endif

