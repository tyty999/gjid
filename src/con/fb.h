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
    virtual void		SetMode (CFbMode m, size_t depth = 8);
    virtual void		SetStandardMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60);
    virtual void		CheckEvents (CEventProcessor* evp) const;
    virtual void		OnFocus (bool bFocused);
    virtual void		Flush (void);
protected:
				CConsoleFramebuffer (void);
    virtual		       ~CConsoleFramebuffer (void);
    void			DetectDefaultDevice (string& deviceName) const;
    void			SetColormap (void);
    void			SetScreeninfo (const struct fb_var_screeninfo& newInfo);
private:
    struct fb_fix_screeninfo	m_Fix;
    struct fb_var_screeninfo	m_OrigVar;
    struct fb_var_screeninfo	m_Var;
    CFile			m_Device;
    memlink			m_Screen;
    CColormap			m_Colormap;
};

} // namespace fbgl

#endif

