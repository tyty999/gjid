// pal.cc
//

#include "pal.h"

namespace fbgl {

/// Default constructor.
CPalette::CPalette (void)
: m_Data ()
{
}

/// Sets color \p i to given components.
void CPalette::Set (color_t i, ray_t r, ray_t g, ray_t b)
{
    palvec_t::iterator ii (m_Data.begin() + i * 3);
    *ii = r;
    *++ii = g;
    *++ii = b;
}

/// Returns components of color \p i.
void CPalette::Get (color_t i, ray_t& r, ray_t& g, ray_t& b) const
{
    palvec_t::const_iterator ii (m_Data.begin() + i * 3);
    r = *ii;
    g = *++ii;
    b = *++ii;
}

} // namespace fbgl

