// fb.h
//

#ifndef FB_H_7338CBBF13B535483C54DF471B2E407C
#define FB_H_7338CBBF13B535483C54DF471B2E407C

#include "fbmode.h"
#include "evp.h"
#include "gc.h"

namespace fbgl {

enum EStdFbMode {
    stdmode_Text,
    stdmode_320x240x8,
    stdmode_640x480x8,
    stdmode_Last
};

/// \class CFramebuffer fb.h fbgl/fb.h
/// \brief Abstract framebuffer interface.
class CFramebuffer {
public:
    virtual void		Open (void) = 0;
    virtual void		Close (void) = 0;
    virtual void		CheckEvents (CEventProcessor* evp) const = 0;
    virtual void		Flush (void) = 0;
    virtual void		OnFocus (bool bFocus);
    virtual void		SetMode (CFbMode m, size_t depth = 8);
    virtual void		SetStandardMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60);
    const CFbMode&		FindClosestMode (size_t w, size_t h, size_t freq) const;
    void			LoadModes (void);
    inline const CGC&		GC (void) const	{ return (m_GC); }
    inline CGC&			GC (void)	{ return (m_GC); }
protected:
				CFramebuffer (void);
    virtual		       ~CFramebuffer (void);
private:
    typedef vector<CFbMode>	modevec_t;
private:
    modevec_t			m_Modes;
    CGC				m_GC;
    memblock			m_Buffer;
};

} // namespace fbgl

#endif

