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
			CPalette (void);
    inline void		read (istream& is)		{ is >> m_Data; }
    inline void		write (ostream& os) const	{ os << m_Data; }
    inline size_t	stream_size (void) const	{ return (stream_size_of (m_Data)); }
    void		Set (color_t i, ray_t r, ray_t g, ray_t b);
    void		Get (color_t i, ray_t& r, ray_t& g, ray_t& b) const;
private:
    typedef tuple<256*3,ray_t>	palvec_t;
private:
    palvec_t		m_Data;
};

} // namespace fbgl

STD_STREAMABLE (fbgl::CPalette)

#endif

