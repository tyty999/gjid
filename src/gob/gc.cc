// gc.cc
//

#include "gc.h"

namespace fbgl {

/// Default constructor.
CGC::CGC (void)
: m_Pix (),
  m_Size ()
{
}

/// Attaches to pixels at \p l of size \p sz.
void CGC::link (memlink l, Size2d sz)
{
    m_Pix.link (l);
    m_Size = sz;
}

/// Detaches from the current pixel data.
void CGC::unlink (void)
{
    m_Pix.unlink();
    m_Size = Size2d();
}

} // namespace fbgl

