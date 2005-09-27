// fbdrv.h
//

#ifndef FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670
#define FBDRV_H_55DF359B07B17DD34D71BCFE3BC0D670

#include "fbmode.h"

namespace fbgl {

class CFramebuffer {
public:
   static CFramebuffer&	Instance (void);
    void		Open (void);
    void		Close (void);
    void		SetMode (size_t w, size_t h, size_t depth, size_t freq);
private:
    typedef vector<CFbMode>	modevec_t;
private:
			CFramebuffer (void);
    void		LoadModes (void);
    uoff_t		FindClosestMode (size_t w, size_t h, size_t freq) const;
private:
    modevec_t		m_Modes;
};

} // namespace fbgl

#endif

