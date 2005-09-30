// fb.h
//

#ifndef FB_H_7338CBBF13B535483C54DF471B2E407C
#define FB_H_7338CBBF13B535483C54DF471B2E407C

#include "fbmode.h"
#include "evp.h"
#include "gc.h"

namespace fbgl {

/// \class CFramebuffer fb.h fbgl/fb.h
/// \brief Abstract framebuffer interface.
class CFramebuffer {
public:
    virtual void		Open (void) = 0;
    virtual void		Close (void) = 0;
    virtual void		SetMode (CFbMode m, size_t depth) = 0;
    virtual const CFbMode&	FindClosestMode (size_t w, size_t h, size_t freq) const = 0;
    virtual void		OnFocus (bool bFocus) = 0;
    virtual void		CheckEvents (CEventProcessor* evp) const = 0;
    virtual memlink		Pixels (void) = 0;
    virtual Size2d		Size (void) = 0;
    virtual void		Flush (const CGC& gc) = 0;
protected:
    inline			CFramebuffer (void) {}
    inline virtual	       ~CFramebuffer (void) {}
};

} // namespace fbgl

#endif

