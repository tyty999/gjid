// fbdrv.h
//

#ifndef FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670
#define FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670

#include "../fb.h"
#include "cmap.h"

namespace fbgl {

class CConsoleFramebuffer : public CFramebuffer {
public:
    static CConsoleFramebuffer&	Instance (void);
    virtual void		Open (void);
    virtual void		Close (void);
    virtual void		SetMode (CFbMode m, size_t depth);
    virtual const CFbMode&	FindClosestMode (size_t w, size_t h, size_t freq) const;
    virtual void		OnFocus (bool bFocus);
    virtual void		CheckEvents (CEventProcessor* evp) const;
    virtual memlink		Pixels (void);
    virtual Size2d		Size (void);
    virtual void		Flush (const CGC& gc);
protected:
				CConsoleFramebuffer (void);
    virtual		       ~CConsoleFramebuffer (void);
    void			LoadModes (void);
    void			DetectDefaultDevice (string& deviceName) const;
    void			SetColormap (void);
private:
    typedef vector<CFbMode>	modevec_t;
private:
    struct fb_fix_screeninfo	m_Fix;
    struct fb_var_screeninfo	m_OrigVar;
    struct fb_var_screeninfo	m_Var;
    CFile			m_Device;
    modevec_t			m_Modes;
    memlink			m_Screen;
    CColormap			m_Colormap;
    static const CFbMode	s_NullMode;
};

} // namespace fbgl

#endif

