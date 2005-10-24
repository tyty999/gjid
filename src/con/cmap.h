// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// cmap.h
//

#ifndef CMAP_H_7738C1BC229459A94DB9E3E048568B33
#define CMAP_H_7738C1BC229459A94DB9E3E048568B33

#include "../pal.h"
#include <linux/fb.h>

namespace fbgl {

/// \class CColormap cmap.h "render/fb/cmap.h"
class DLL_LOCAL CColormap : public fb_cmap {
public:
    typedef const struct fb_var_screeninfo&	rcvarinfo_t;
    typedef const struct fb_fix_screeninfo&	rcfixinfo_t;
public:
		CColormap (void);
	       ~CColormap (void);
    void	Resize (size_t bpp, size_t rlen, size_t glen, size_t blen, bool bDirectColor);
    void	InitTruecolorValues (size_t bpp, size_t rlen, size_t glen, size_t blen, bool bDirectColor);
    void	CopyFrom (const CPalette& pal);
    void	CopyTo (CPalette& pal) const;
private:
    void	InitTruecolorRamp (__u16* v, size_t bits, bool bDirectColor);
};

} // namespace fbgl

#endif

