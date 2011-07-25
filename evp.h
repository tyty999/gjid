// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "pal.h"
#include <X11/keysym.h>

enum {
    ModKeyReleasedMask = Mod4Mask,
    _XKM_Bitshift = 24,
    XKM_Shift = ShiftMask << _XKM_Bitshift,
    XKM_Ctrl = ControlMask << _XKM_Bitshift,
    XKM_Alt = Mod1Mask << _XKM_Bitshift,
    XKM_Released = ModKeyReleasedMask << _XKM_Bitshift,
    XKM_Mask = XKM_Shift| XKM_Ctrl| XKM_Alt| XKM_Released
};

/// \class CEventProcessor evp.h fbgl.h
///
/// \brief Event processor interface.
///
class CEventProcessor {
public:
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
public:
    inline virtual	~CEventProcessor (void) {}
    inline virtual void	OnKey (key_t) {}
    inline virtual void	OnMouseMove (coord_t, coord_t) {}
    inline virtual void	OnButtonDown (bidx_t, coord_t, coord_t) {}
};
