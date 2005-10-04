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

void CXlibFramebuffer::SetMode (CFbMode, size_t)
{
}

void CXlibFramebuffer::OnFocus (bool)
{
}

void CXlibFramebuffer::CheckEvents (CEventProcessor*) const
{
}

memlink CXlibFramebuffer::Pixels (void)
{
    return (memlink());
}

Size2d CXlibFramebuffer::Size (void)
{
    return (Size2d());
}

void CXlibFramebuffer::Flush (const CGC&)
{
}

} // namespace fbgl

