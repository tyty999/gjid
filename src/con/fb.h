// fbdrv.h
//

#ifndef FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670
#define FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670

#include "fbmode.h"

struct fb_fix_screeninfo;
struct fb_var_screeninfo;

namespace fbgl {

class CFramebuffer {
public:
    static CFramebuffer&	Instance (void);
    void			Open (void);
    void			Close (void);
    void			SetMode (size_t w, size_t h, size_t freq, size_t depth);
private:
    typedef vector<CFbMode>	modevec_t;
private:
				CFramebuffer (void);
    virtual		       ~CFramebuffer (void);
    void			LoadModes (void);
    uoff_t			FindClosestMode (size_t w, size_t h, size_t freq) const;
    void			DetectDefaultDevice (string& deviceName) const;
private:
    struct fb_fix_screeninfo*	m_pFix;
    struct fb_var_screeninfo*	m_pOrigVar;
    struct fb_var_screeninfo*	m_pVar;
    CFile			m_Device;
    modevec_t			m_Modes;
    memlink			m_Screen;
};

} // namespace fbgl

#endif

