// xfb.h
//

#ifndef XFB_H_03AB4F7E09259B61062C6ACD0AB3CC92
#define XFB_H_03AB4F7E09259B61062C6ACD0AB3CC92

#include "../fb.h"

namespace fbgl {

/// \class CXlibFramebuffer xfb fbgl.h
///
/// \brief Xlib backend.
///
class CXlibFramebuffer : public CFramebuffer {
public:
    static CXlibFramebuffer&	Instance (void);
    virtual void		Open (void);
    virtual void		Close (void);
    virtual void		SetMode (CFbMode m, size_t depth);
    virtual const CFbMode&	FindClosestMode (size_t w, size_t h, size_t freq) const;
    virtual void		OnFocus (bool bFocus);
    virtual void		CheckEvents (CEventProcessor* evp) const;
    virtual memlink		Pixels (void);
    virtual Size2d		Size (void);
    virtual void		Flush (const CGC& gc);
    void			LoadModes (void);
protected:
				CXlibFramebuffer (void);
    virtual		       ~CXlibFramebuffer (void);
private:
    typedef vector<CFbMode>	modevec_t;
private:
    modevec_t			m_Modes;
};

} // namespace fbgl

#endif

