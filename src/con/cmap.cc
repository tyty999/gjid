// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// cmap.cc
//

#include "cmap.h"

namespace fbgl {

/// Default constructor.
CColormap::CColormap (void)
{
    start = 0;
    len = 0;
    red = NULL;
    green = NULL;
    blue = NULL;
    transp = NULL;
}

/// Destructor. Frees the raymaps.
CColormap::~CColormap (void)
{
    #define FreeIfAllocated(ray)\
	if (ray) {		\
	    free (ray);		\
	    ray = NULL;		\
	}
    FreeIfAllocated (red);
    FreeIfAllocated (green);
    FreeIfAllocated (blue);
    #undef FreeIfAllocated
}

/// Resizes the raymaps to hold enough elements for \p rb.
void CColormap::Resize (rcvarinfo_t rv, rcfixinfo_t rf)
{
    size_t nReds, nGreens, nBlues;
    if (rf.visual == FB_VISUAL_DIRECTCOLOR) {
	nReds   = 1 << rv.red.length;
	nGreens = 1 << rv.green.length;
	nBlues  = 1 << rv.blue.length;
    } else
	nReds = nGreens = nBlues = 1 << rv.bits_per_pixel;
    len = max (max (nReds, nGreens), nBlues);
    red   = (__u16*) realloc (red,   nReds * sizeof (__u16));
    green = (__u16*) realloc (green, nGreens * sizeof (__u16));
    blue  = (__u16*) realloc (blue,  nBlues * sizeof (__u16));
    fill_n (red, nReds, 0U);
    fill_n (green, nReds, 0U);
    fill_n (blue, nReds, 0U);
}

/// Fills \p v with truecolor values spread over \p bits.
void CColormap::InitTruecolorRamp (__u16* v, size_t bits, bool bDirectColor)
{
    const size_t nEntries = (1U << bits);
    const size_t scale = (bDirectColor ? 8 : 16) - bits;	// DirectColor visuals use only 8 bits of the colormap.
    for (uoff_t i = 0; i < nEntries; ++ i)
	v[i] = i << scale;
    if (bDirectColor)						// ... and there is no standard as to which 8 bits.
	for (uoff_t i = 0; i < nEntries; ++ i)
	    v[i] |= v[i] << 8;
}

/// Fills the colormap with a truecolor-like ramp clamped to \p rb.
void CColormap::InitTruecolorValues (rcvarinfo_t rv, rcfixinfo_t rf)
{
    Resize (rv, rf);
    size_t nReds, nGreens, nBlues;
    if (rf.visual == FB_VISUAL_DIRECTCOLOR) {
	nReds   = rv.red.length;
	nGreens = rv.green.length;
	nBlues  = rv.blue.length;
    } else
	nReds = nGreens = nBlues = rv.bits_per_pixel;
    InitTruecolorRamp (red, nReds, rf.visual == FB_VISUAL_DIRECTCOLOR);
    InitTruecolorRamp (green, nGreens, rf.visual == FB_VISUAL_DIRECTCOLOR);
    InitTruecolorRamp (blue, nBlues, rf.visual == FB_VISUAL_DIRECTCOLOR);
}

//----------------------------------------------------------------------

} // namespace fbgl

