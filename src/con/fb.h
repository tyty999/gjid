// fbdrv.h
//

#ifndef FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670
#define FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670

#include "fbmode.h"
#include "cmap.h"

namespace fbgl {

class CFramebuffer {
public:
    static CFramebuffer&	Instance (void);
    void			Open (void);
    void			Close (void);
    void			SetMode (CFbMode m, size_t depth);
    const CFbMode&		FindClosestMode (size_t w, size_t h, size_t freq) const;
private:
    typedef vector<CFbMode>	modevec_t;
private:
				CFramebuffer (void);
    virtual		       ~CFramebuffer (void);
    void			LoadModes (void);
    void			DetectDefaultDevice (string& deviceName) const;
    void			SetColormap (void);
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

