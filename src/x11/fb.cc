// xfb.cc
//

#include "xfb.h"

namespace fbgl {

CXlibFramebuffer::CXlibFramebuffer (void)
: CFramebuffer ()
{
}

CXlibFramebuffer::~CXlibFramebuffer (void)
{
}

/*static*/ CXlibFramebuffer& CXlibFramebuffer::Instance (void)
{
    static CXlibFramebuffer s_Fb;
    return (s_Fb);
}

void CXlibFramebuffer::Open (void)
{
}

void CXlibFramebuffer::Close (void)
{
}

void CXlibFramebuffer::SetMode (CFbMode m, size_t depth)
{
    CFramebuffer::SetMode (m, depth);
}

void CXlibFramebuffer::CheckEvents (CEventProcessor*) const
{
}

void CXlibFramebuffer::Flush (void)
{
}

} // namespace fbgl

