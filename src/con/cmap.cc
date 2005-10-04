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
void CColormap::Resize (size_t bpp, size_t rlen, size_t glen, size_t blen, bool bDirectColor)
{
    if (!bDirectColor)
	rlen = glen = blen = bpp;
    rlen = 1 << rlen;
    glen = 1 << glen;
    blen = 1 << blen;
    len = max (max (rlen, glen), blen);
    red   = (__u16*) realloc (red,   rlen * sizeof (__u16));
    green = (__u16*) realloc (green, glen * sizeof (__u16));
    blue  = (__u16*) realloc (blue,  blen * sizeof (__u16));
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
void CColormap::InitTruecolorValues (size_t bpp, size_t rlen, size_t glen, size_t blen, bool bDirectColor)
{
    Resize (bpp, rlen, glen, blen, bDirectColor);
    if (!bDirectColor)
	rlen = glen = blen = bpp;
    InitTruecolorRamp (red, rlen, bDirectColor);
    InitTruecolorRamp (green, glen, bDirectColor);
    InitTruecolorRamp (blue, blen, bDirectColor);
}

/// Create a pseudocolor palette from \p pal.
void CColormap::CopyFrom (const CPalette& pal)
{
    Resize (8, 8, 8, 8, false);
    ray_t* pr (((ray_t*) red));
    ray_t* pg (((ray_t*) green));
    ray_t* pb (((ray_t*) blue));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ++ pr; ++ pg; ++ pb;
#endif
    for (uoff_t i = 0; i < 256; ++i, pr+=2, pg+=2, pb+=2)
	pal.Get (i, *pr, *pg, *pb);
}

void CColormap::CopyTo (CPalette& pal) const
{
    const ray_t* pr (((const ray_t*) red));
    const ray_t* pg (((const ray_t*) green));
    const ray_t* pb (((const ray_t*) blue));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ++ pr; ++ pg; ++ pb;
#endif
    for (uoff_t i = 0; i < 256; ++i, pr+=2, pg+=2, pb+=2)
	pal.Set (i, *pr, *pg, *pb);
}

//----------------------------------------------------------------------

} // namespace fbgl

